#include "CommonPCH.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryDefault.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryRender.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryDefault.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryRender.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/Log/Log.h"
#include "json/json.hpp"

/*
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/Interface/IUpdate.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONShaderResource.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONRenderTarget.h"
#include "Common/Math/MatrixFloat33.h"
#include "Common/Math/QuaternionFloat.h"
#include "Common/Math/VectorFloat3.h"
#include "Common/Math/VectorFloat4.h"
#include "Common/Math/VectorMath.h"
*/


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

	const std::map<std::string, NodeValueFactory> FactoryMapValue(DrawSystem* pDrawSystem)
	{
		std::map<std::string, NodeValueFactory> mapValue;

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

	m_pDagDrawSystemFrame = DagNodeValue::Factory(
		DagValue<DrawSystemFrame*>::Factory(nullptr),
		false
	);
	m_pDagBackBuffer = DagNodeValue::Factory(DagValue<IRenderTarget*>::Factory(nullptr));
	m_pDagTimeAccumulate = DagNodeValue::Factory(DagValue<float>::Factory(0));

	auto mapValue = FactoryMapValue(m_pDrawSystem.get());
	auto mapCalculate = FactoryMapCalculate(m_pDrawSystem.get(), applicationParam);

	std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues{
		{ "_DrawSystemFrame", m_pDagDrawSystemFrame },
		{ "_BackBuffer", m_pDagBackBuffer },
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

	if (m_pDrawSystem)
	{
		auto pFrame = m_pDrawSystem->CreateNewFrame();

		DagValue<DrawSystemFrame*>::UpdateNode(m_pDagDrawSystemFrame, pFrame.get());
		DagValue<IRenderTarget*>::UpdateNode(m_pDagBackBuffer, m_pDrawSystem->GetRenderTargetBackBuffer());

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
