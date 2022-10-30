#include "CommonPCH.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/RenderTarget/IRenderTarget.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryDefault.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryRender.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryAxis.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryDefault.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryRender.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeRef.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/Interface/IUpdate.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/Log/Log.h"
#include "json/json.hpp"

class JSONApplicationDisplayList
{
public:
	std::string file;
	JSONDrawSystem drawSystem;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayList,
	file,
	drawSystem
);

IApplication* const ApplicationDisplayList::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
	return new ApplicationDisplayList(hWnd, applicationParam);
}

namespace
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

	const std::map<std::string, NodeValueFactory> FactoryMapValue(
		DrawSystem* pDrawSystem,
		std::vector< std::shared_ptr< IUpdate > >* pUpdatableObjects
		)
	{
		std::map<std::string, NodeValueFactory> mapValue;

		NodeValueFactoryAxis::Append(mapValue, pUpdatableObjects);
		NodeValueFactoryDefault::Append(mapValue);
		NodeValueFactoryRender::Append(mapValue, pDrawSystem);

		return mapValue;
	}

	const std::map<std::string, NodeCalculateFactory> FactoryMapCalculate(DrawSystem* pDrawSystem, const IApplicationParam& applicationParam)
	{
		std::map<std::string, NodeCalculateFactory> mapCalculate;

		NodeCalculateFactoryDefault::Append(mapCalculate);
		NodeCalculateFactoryRender::Append(mapCalculate, applicationParam.m_rootPath, pDrawSystem);

		return mapCalculate;
	}
};

ApplicationDisplayList::ApplicationDisplayList(const HWND hWnd, const IApplicationParam& applicationParam)
	: IApplication(hWnd, applicationParam)
	, m_timePointValid(false)
	, m_timePoint()
{
	LOG_MESSAGE("ApplicationDisplayList ctor %p", this);

	JSONApplicationDisplayList jsonData;
	nlohmann::from_json(applicationParam.m_json, jsonData);

	m_pDrawSystem = DrawSystem::Factory(hWnd, jsonData.drawSystem);

	//a NodeRef rather than NodeValue, as it should not set the graph dirty on being set
	m_pDagDrawSystemFrame = DagNodeRef::Factory(DagValue<DrawSystemFrame*>::Factory(nullptr));
	//a NodeRef rather than NodeValue, as it should not set the graph dirty on being set
	m_pDagBackBuffer = DagNodeRef::Factory(DagValue<IRenderTarget*>::Factory(nullptr));

	//NodeValue, change of these mark dependants dirty
	m_pDagLayoutWidth = DagNodeValue::Factory(DagValue<float>::Factory((float)applicationParam.m_width)); 
	m_pDagLayoutHeight = DagNodeValue::Factory(DagValue<float>::Factory((float)applicationParam.m_height)); 
	m_pDagTimeAccumulate = DagNodeValue::Factory(DagValue<float>::Factory(0));

	auto mapValue = FactoryMapValue(m_pDrawSystem.get(), &m_updatableObjects);
	auto mapCalculate = FactoryMapCalculate(m_pDrawSystem.get(), applicationParam);

	std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues{
		{ "_DrawSystemFrame", m_pDagDrawSystemFrame },
		{ "_BackBuffer", m_pDagBackBuffer },
		{ "_LayoutWidth", m_pDagLayoutWidth },
		{ "_LayoutHeight", m_pDagLayoutHeight },
		{ "_TimeAccumulate", m_pDagTimeAccumulate }
	};

	m_pDagCollection = JSONDagCollection::Factory(
		jsonData.file,
		[=](const std::string& fileName) {
			auto newPath = applicationParam.m_rootPath / fileName;
			auto pResult = FileSystem::SyncReadFile(newPath);
			auto result = FileSystem::DataToString(pResult);
			return result;
		},
		inbuiltDagValues,
		mapValue,
		mapCalculate
		);
}

ApplicationDisplayList::~ApplicationDisplayList()
{
	if (m_pDrawSystem)
	{
		m_pDrawSystem->WaitForGpu();
	}
	m_pDagCollection.reset();
	m_pDrawSystem.reset();

	LOG_MESSAGE("ApplicationDisplayList dtor %p", this);
}

void ApplicationDisplayList::Update()
{
	BaseType::Update();

	const auto timePointNow = std::chrono::system_clock::now();
	float timeDeltaSeconds = 0.0f;
	if (true == m_timePointValid)
	{
		timeDeltaSeconds = (1.0f / 1000.0f) * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_timePoint).count());
	}
	else
	{
		m_timePointValid = true;
	}
	m_timePoint = timePointNow;
	if (m_pDagTimeAccumulate)
	{
		DagValue<float>::IncrementNode(m_pDagTimeAccumulate, timeDeltaSeconds);
	}

	for (auto& item: m_updatableObjects)
	{
		item->Update(timeDeltaSeconds);
	}

	if (m_pDrawSystem)
	{
		auto pFrame = m_pDrawSystem->CreateNewFrame();

		DagValue<DrawSystemFrame*>::UpdateNode(m_pDagDrawSystemFrame, pFrame.get());
		auto pBackBuffer = m_pDrawSystem->GetRenderTargetBackBuffer();
		DagValue<IRenderTarget*>::UpdateNode(m_pDagBackBuffer, pBackBuffer);
		DagValue<float>::UpdateNode(m_pDagLayoutWidth, (float)(pBackBuffer ? pBackBuffer->GetWidth() : 0.0f));
		DagValue<float>::UpdateNode(m_pDagLayoutHeight, (float)(pBackBuffer ? pBackBuffer->GetHeight() : 0.0f));

		auto pRender = m_pDagCollection->GetDagNode("_Render");
		if (pRender)
		{
			pRender->GetValue();
		}
	}
}

void ApplicationDisplayList::OnWindowSizeChanged(const int width, const int height)
{
	BaseType::OnWindowSizeChanged(width, height);
	if (m_pDrawSystem)
	{
		m_pDrawSystem->OnResize();
	}

	return;
}
