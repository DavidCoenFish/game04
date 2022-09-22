#include "CommonPCH.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/Interface/IUpdate.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONShaderResource.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONRenderTarget.h"
#include "Common/Log/Log.h"
#include "Common/Math/MatrixFloat33.h"
#include "Common/Math/QuaternionFloat.h"
#include "Common/Math/VectorFloat3.h"
#include "Common/Math/VectorFloat4.h"
#include "Common/Math/VectorMath.h"
#include "json/json.hpp"

class UpdateTaskSetScreenWidthHeight : public IUpdate
{
public:
	UpdateTaskSetScreenWidthHeight(
		iDagNode* const pTarget,
		const std::vector<int>& offsetArray,
		DrawSystem* const pDrawSystem
	)
		: m_pTarget(pTarget)
		, m_offsetArray(offsetArray)
		, m_pDrawSystem(pDrawSystem)
	{
		DSC_ASSERT(2 == m_offsetArray.size());
		DSC_ASSERT(0 <= m_offsetArray[0]);
		DSC_ASSERT(0 <= m_offsetArray[1]);
		return;
	}
private:
	virtual void Update(const float) override
	{
		auto pValue = m_pTarget ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(m_pTarget->GetValue()) : nullptr;
		if (nullptr == pValue)
		{
			return;
		}
		std::vector<float> valueArray = pValue->GetRef();
		int valueSize = (int)valueArray.size();
		auto pRenderTarget = m_pDrawSystem ? m_pDrawSystem->GetRenderTargetBackBuffer() : nullptr;
		if ((nullptr != pRenderTarget) && (m_offsetArray[0] < valueSize) && (m_offsetArray[1] < valueSize))
		{
			valueArray[m_offsetArray[0]] = (float)(pRenderTarget->GetWidth());
			valueArray[m_offsetArray[1]] = (float)(pRenderTarget->GetHeight());

			if (pValue->GetRef() == valueArray)
			{
				return;
			}
			pValue->SetRef(valueArray);
			m_pTarget->SetValue(pValue);
		}
	}
private:
	iDagNode* const m_pTarget;
	std::vector<int> m_offsetArray;
	DrawSystem* const m_pDrawSystem;
};

class JSONApplicationDisplayListSourceMove
{
public:
	std::string name;
	std::vector< int > atOffset;
	std::vector< int > upOffset;
	std::vector< int > scancode;
	float velocity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayListSourceMove,
	name,
	atOffset,
	upOffset,
	scancode,
	velocity
);

class UpdateTaskInputMove : public IUpdate
{
public:
	UpdateTaskInputMove(
		iDagNode* const pTarget,
		const std::vector<int>& offsetArray,
		iDagNode* const pSource,
		const JSONApplicationDisplayListSourceMove& jsonSourceMove,
		std::map<int, int>& scanCodeRepeatMap
	)
		: m_pTarget(pTarget)
		, m_offsetArray(offsetArray)
		, m_pSource(pSource)
		, m_jsonSourceMove(jsonSourceMove)
		, m_scanCodeRepeatMap(scanCodeRepeatMap)
	{
		DSC_ASSERT(3 == m_offsetArray.size());
		DSC_ASSERT(0 <= m_offsetArray[0]);
		DSC_ASSERT(0 <= m_offsetArray[1]);
		DSC_ASSERT(0 <= m_offsetArray[2]);
		DSC_ASSERT(3 == m_jsonSourceMove.atOffset.size());
		DSC_ASSERT(3 == m_jsonSourceMove.upOffset.size());
		return;
	}

private:
	virtual void Update(const float in_timeDeltaSeconds) override
	{
		in_timeDeltaSeconds;
		auto pValueTarget = m_pTarget ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(m_pTarget->GetValue()) : nullptr;
		auto pValueSource = m_pSource ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(m_pSource->GetValue()) : nullptr;
		if ((nullptr == pValueTarget) || (nullptr == pValueSource))
		{
			return;
		}
		std::vector<float> valueArrayTarget = pValueTarget->GetRef();
		const std::vector<float>& valueArraySource = pValueSource->GetRef();

		const float at[3] = {
			valueArraySource[m_jsonSourceMove.atOffset[0]],
			valueArraySource[m_jsonSourceMove.atOffset[1]],
			valueArraySource[m_jsonSourceMove.atOffset[2]]
		};
		const float up[3] = {
			valueArraySource[m_jsonSourceMove.upOffset[0]],
			valueArraySource[m_jsonSourceMove.upOffset[1]],
			valueArraySource[m_jsonSourceMove.upOffset[2]]
		};


		const int upDownRepeat = m_scanCodeRepeatMap[m_jsonSourceMove.scancode[4]] - m_scanCodeRepeatMap[m_jsonSourceMove.scancode[5]];
		if (0 != upDownRepeat)
		{
			const float offset = (float)upDownRepeat * in_timeDeltaSeconds * m_jsonSourceMove.velocity;
			valueArrayTarget[m_offsetArray[0]] += offset * up[0];
			valueArrayTarget[m_offsetArray[1]] += offset * up[1];
			valueArrayTarget[m_offsetArray[2]] += offset * up[2];
		}

		const int forwardBackRepeat = m_scanCodeRepeatMap[m_jsonSourceMove.scancode[0]] - m_scanCodeRepeatMap[m_jsonSourceMove.scancode[2]];
		if (0 != forwardBackRepeat)
		{
			const float offset = (float)forwardBackRepeat * in_timeDeltaSeconds * m_jsonSourceMove.velocity;
			valueArrayTarget[m_offsetArray[0]] += offset * at[0];
			valueArrayTarget[m_offsetArray[1]] += offset * at[1];
			valueArrayTarget[m_offsetArray[2]] += offset * at[2];
		}

		const int leftRightRepeat = m_scanCodeRepeatMap[m_jsonSourceMove.scancode[1]] - m_scanCodeRepeatMap[m_jsonSourceMove.scancode[3]];
		if (0 != leftRightRepeat)
		{
			float cross[3];
			VectorMath::CrossProduct(cross, at, up);

			const float offset = (float)leftRightRepeat * in_timeDeltaSeconds * m_jsonSourceMove.velocity;
			valueArrayTarget[m_offsetArray[0]] += offset * cross[0];
			valueArrayTarget[m_offsetArray[1]] += offset * cross[1];
			valueArrayTarget[m_offsetArray[2]] += offset * cross[2];
		}

		if (pValueTarget->GetRef() == valueArrayTarget)
		{
			return;
		}

		pValueTarget->SetRef(valueArrayTarget);
		m_pTarget->SetValue(pValueTarget);
	}
private:
	iDagNode* const m_pTarget;
	std::vector<int> m_offsetArray;
	iDagNode* const m_pSource;
	JSONApplicationDisplayListSourceMove m_jsonSourceMove;
	std::map<int, int>& m_scanCodeRepeatMap;

};

class JSONApplicationDisplayListSourceRotate
{
public:
	std::string name;
	std::vector< int > atOffset;
	std::vector< int > upOffset;
	std::vector< int > scancode;
	float velocity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayListSourceRotate,
	name,
	atOffset,
	upOffset,
	scancode,
	velocity
);

class UpdateTaskInputRotate : public IUpdate
{
public:
	UpdateTaskInputRotate(
		iDagNode* const pTarget,
		const std::vector<int>& offsetArray,
		iDagNode* const pSource,
		const JSONApplicationDisplayListSourceRotate& jsonSourceRotate,
		std::map<int, int>& scanCodeRepeatMap
	)
		: m_pTarget(pTarget)
		, m_offsetArray(offsetArray)
		, m_pSource(pSource)
		, m_jsonSourceRotate(jsonSourceRotate)
		, m_scanCodeRepeatMap(scanCodeRepeatMap)
	{
		DSC_ASSERT(6 == m_offsetArray.size());
		DSC_ASSERT(3 == m_jsonSourceRotate.atOffset.size());
		DSC_ASSERT(3 == m_jsonSourceRotate.upOffset.size());
		return;
	}

private:
	virtual void Update(const float in_timeDeltaSeconds) override
	{
		auto pValueTarget = m_pTarget ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(m_pTarget->GetValue()) : nullptr;
		auto pValueSource = m_pSource ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(m_pSource->GetValue()) : nullptr;
		if ((nullptr == pValueTarget) || (nullptr == pValueSource))
		{
			return;
		}
		std::vector<float> valueArrayTarget = pValueTarget->GetRef();
		const std::vector<float>& valueArraySource = pValueSource->GetRef();

		const VectorFloat3 at(
			valueArraySource[m_jsonSourceRotate.atOffset[0]],
			valueArraySource[m_jsonSourceRotate.atOffset[1]],
			valueArraySource[m_jsonSourceRotate.atOffset[2]]
		);
		const VectorFloat3 up(
			valueArraySource[m_jsonSourceRotate.upOffset[0]],
			valueArraySource[m_jsonSourceRotate.upOffset[1]],
			valueArraySource[m_jsonSourceRotate.upOffset[2]]
		);
		const VectorFloat3 cross = VectorFloat3::Cross(at, up);

		bool bActive = false;

		//[ "arrow_up", "arrow_left", "arrow_down", "arrow_right", "pageup", "pagedown" ],
		auto rotation = QuaternionFloat::FactoryIdentity();
		const int yawRepeat = m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[1]] - m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[3]];
		if (0 != yawRepeat)
		{
			const float offset = (float)yawRepeat * in_timeDeltaSeconds * m_jsonSourceRotate.velocity;
			auto mul = QuaternionFloat::FactoryAxisAngle(up, offset);
			rotation = rotation * mul;
			bActive = true;
		}

		const int pitchRepeat = m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[0]] - m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[2]];
		if (0 != pitchRepeat)
		{
			const float offset = (float)pitchRepeat * in_timeDeltaSeconds * m_jsonSourceRotate.velocity;
			auto mul = QuaternionFloat::FactoryAxisAngle(cross, offset);
			rotation = rotation * mul;
			bActive = true;
		}

		const int rollRepeat = m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[4]] - m_scanCodeRepeatMap[m_jsonSourceRotate.scancode[5]];
		if (0 != rollRepeat)
		{
			const float offset = (float)rollRepeat * in_timeDeltaSeconds * m_jsonSourceRotate.velocity;
			auto mul = QuaternionFloat::FactoryAxisAngle(at, offset);
			rotation = rotation * mul;
			bActive = true;
		}

		if (true == bActive)
		{
			const auto matrix = MatrixFloat33::FactoryQuaternionFloat(rotation);
			auto localAt = matrix * at;
			auto localCross = matrix * cross;
			auto localUp = VectorFloat3::Cross(localCross, localAt);

			localAt.NormaliseSelf();
			localUp.NormaliseSelf();

			valueArrayTarget[m_offsetArray[0]] = localAt[0];
			valueArrayTarget[m_offsetArray[1]] = localAt[1];
			valueArrayTarget[m_offsetArray[2]] = localAt[2];

			valueArrayTarget[m_offsetArray[3]] = localUp[0];
			valueArrayTarget[m_offsetArray[4]] = localUp[1];
			valueArrayTarget[m_offsetArray[5]] = localUp[2];
		}

		if (pValueTarget->GetRef() == valueArrayTarget)
		{
			return;
		}

		pValueTarget->SetRef(valueArrayTarget);
		m_pTarget->SetValue(pValueTarget);
	}
private:
	iDagNode* const m_pTarget;
	std::vector<int> m_offsetArray;
	iDagNode* const m_pSource;
	JSONApplicationDisplayListSourceRotate m_jsonSourceRotate;
	std::map<int, int>& m_scanCodeRepeatMap;

};


class JSONApplicationDisplayListTarget
{
public:
	std::string name;
	std::vector< int > offset;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayListTarget,
	name,
	offset
);


enum class JSONApplicationDisplayListUpdateType
{
	SetScreenWidthHeightToDagFloatArray,
	InputRotateToDagFloatArray,
	InputMoveToDagFloatArray
};

NLOHMANN_JSON_SERIALIZE_ENUM(JSONApplicationDisplayListUpdateType, {
	{ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, SetScreenWidthHeightToDagFloatArray)},
	{ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, InputRotateToDagFloatArray)},
	{ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, InputMoveToDagFloatArray)},
	});

class JSONApplicationDisplayListUpdate
{
public:
	JSONApplicationDisplayListUpdateType type;
	nlohmann::json source;
	JSONApplicationDisplayListTarget target;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayListUpdate,
	type,
	source,
	target
);

class JSONHeapWrapperFromRenderTargetData
{
public:
	bool useDepth;
	bool useShaderResource;
	int shaderResourceIndex;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONHeapWrapperFromRenderTargetData,
	useDepth,
	useShaderResource,
	shaderResourceIndex
);

class JSONApplicationDisplayList
{
public:
	std::string file;
	JSONDrawSystem drawSystem;
	std::vector< JSONApplicationDisplayListUpdate > update;
	//std::vector< nlohmann::json > update;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONApplicationDisplayList,
	file,
	drawSystem,
	update
);

std::shared_ptr< std::vector<uint8_t> > LoadFile(const std::filesystem::path& rootPath, const std::string& fileName)
{
	if (true == fileName.empty())
	{
		return nullptr;
	}
	auto newPath = rootPath / fileName;
	auto pResult = FileSystem::SyncReadFile(newPath);
	return pResult;
}

typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;
typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

IApplication* const ApplicationDisplayList::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
	return new ApplicationDisplayList(hWnd, applicationParam);
}

namespace
{
	template <typename TYPE>
	NodeCalculateFactory MakeStackFactory()
	{
		return [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::vector< TYPE > >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::vector< TYPE > >::Factory(std::vector< TYPE >());
				}
				std::vector< TYPE > resultData;
				for (const auto& item : stackInput)
				{
					auto pTemp = item ? std::dynamic_pointer_cast<DagValue< TYPE >>(item->GetValue()) : nullptr;
					if (nullptr == pTemp)
					{
						continue;
					}
					resultData.push_back(pTemp->GetRef());
				}
				pLocalValue->SetRef(resultData);
				return pLocalValue;
				});
			return pResult;
		};
	}

	const std::map<std::string, NodeValueFactory> FactoryMapValue(DrawSystem* pDrawSystem)
	{
		std::map<std::string, NodeValueFactory> mapValue;

		NodeValueFactoryDefault(mapValue);
		NodeValueFactoryRender(mapValue, pDrawSystem);

		return mapValue;
	}

	const std::map<std::string, NodeCalculateFactory> FactoryMapCalculate(DrawSystem* pDrawSystem, const IApplicationParam& applicationParam)
	{
		std::map<std::string, NodeCalculateFactory> mapCalculate;

		//indexInput
		//  0.std::shared_ptr< ShaderResource >
		//output std::shared_ptr< HeapWrapperItem >
		mapCalculate["HeapWrapperFromTexture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< HeapWrapperItem > >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::shared_ptr< HeapWrapperItem > >::Factory(nullptr);
				}

				const auto pDagHeapWrapperItem = 0 < indexInput.size() ? std::dynamic_pointer_cast<DagValue< std::shared_ptr< ShaderResource > >>(indexInput[0]->GetValue()) : nullptr;
				const auto pShaderResource = pDagHeapWrapperItem ? pDagHeapWrapperItem->GetRef() : nullptr;
				std::shared_ptr< HeapWrapperItem > pHeapWrapperItem;
				if (nullptr != pShaderResource)
				{
					pHeapWrapperItem = pShaderResource->GetHeapWrapperItem();
				}

				pLocalValue->SetRef(pHeapWrapperItem);

				return pLocalValue;
				});
			return pResult;
		};

		//indexInput
		//  //0.std::shared_ptr< RenderTargetTexture >, or IRenderTarget?
		//  0.IRenderTarget
		//output std::shared_ptr< HeapWrapperItem >
		mapCalculate["HeapWrapperFromIRenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			JSONHeapWrapperFromRenderTargetData jsonHeapWrapperFromRenderTargetData;
			data.get_to(jsonHeapWrapperFromRenderTargetData);
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< HeapWrapperItem > >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::shared_ptr< HeapWrapperItem > >::Factory(nullptr);
				}

				const auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(indexInput[0]->GetValue()) : nullptr;
				const IRenderTarget* pIRenderTarget = pDag ? pDag->Get() : nullptr;
				const RenderTargetTexture* pRenderTargetTexture = dynamic_cast<const RenderTargetTexture*>(pIRenderTarget);
				std::shared_ptr< HeapWrapperItem > pHeapWrapperItem;
				if (nullptr != pRenderTargetTexture)
				{
					if (true == jsonHeapWrapperFromRenderTargetData.useDepth)
					{
						pHeapWrapperItem = pRenderTargetTexture->GetDepthShaderResourceHeapWrapperItem();
					}
					else if (true == jsonHeapWrapperFromRenderTargetData.useShaderResource)
					{
						pHeapWrapperItem = pRenderTargetTexture->GetShaderResourceHeapWrapperItem(jsonHeapWrapperFromRenderTargetData.shaderResourceIndex);
					}
				}

				pLocalValue->SetRef(pHeapWrapperItem);

				return pLocalValue;
				});
			return pResult;
		};

		//indexInput
		//  0.std::shared_ptr< RenderTargetTexture >, or IRenderTarget?
		//output std::vector< float >
		mapCalculate["WidthHeightFromIRenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue<std::vector<float>>>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::vector< float > >::Factory(std::vector< float >());
				}

				std::vector< float > output({ 0.0f, 0.0f });
				const auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(indexInput[0]->GetValue()) : nullptr;
				const IRenderTarget* pIRenderTarget = pDag ? pDag->Get() : nullptr;
				if (nullptr != pIRenderTarget)
				{
					output[0] = (float)(pIRenderTarget->GetWidth());
					output[1] = (float)(pIRenderTarget->GetHeight());
				}

				pLocalValue->SetRef(output);

				return pLocalValue;
				});
			return pResult;
		};

		//indexInput
		//  0.std::shared_ptr< RenderTargetTexture >
		//output IRenderTarget
		mapCalculate["IRenderTargetFromRenderTargetTexture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue<IRenderTarget*>>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue<IRenderTarget*>::Factory(nullptr);
				}

				auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast<DagValue< std::shared_ptr< RenderTargetTexture > >>(indexInput[0]->GetValue()) : nullptr;
				RenderTargetTexture* pRenderTargetTexture = pDag ? pDag->Get().get() : nullptr;
				pLocalValue->Set(pRenderTargetTexture);

				return pLocalValue;
				});
			return pResult;
		};

		//stackInput [std::shared_ptr< HeapWrapperItem >]
		//output std::vector< std::shared_ptr< HeapWrapperItem > >
		mapCalculate["StackHeapWrapper"] = MakeStackFactory<std::shared_ptr<HeapWrapperItem>>();

		//stackInput [float]
		//output std::vector< float >
		mapCalculate["StackFloat"] = MakeStackFactory<float>();

		//stackInput [std::vector< float >]
		//output std::vector< std::vector< float > >
		mapCalculate["StackVectorFloat"] = MakeStackFactory<std::vector< float >>();

		//indexInput
		//  0.std::vector< std::shared_ptr< HeapWrapperItem > >
		//  1.std::vector< std::vector< float > >
		//output std::shared_ptr< Shader >
		mapCalculate["Shader"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			JSONShader jsonShader;
			data.get_to(jsonShader);
			auto pCommandList = pDrawSystem->CreateCustomCommandList();
			auto pShader = pDrawSystem->MakeShader(
				pCommandList->GetCommandList(),
				jsonShader.pipelineState,
				LoadFile(applicationParam.m_rootPath, jsonShader.vertexShader),
				LoadFile(applicationParam.m_rootPath, jsonShader.geometryShader),
				LoadFile(applicationParam.m_rootPath, jsonShader.pixelShader),
				TransformShaderResourceInfo(jsonShader.resourceInfo),
				TransformConstantBufferInfo(jsonShader.constantInfo)
			);
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< Shader > >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::shared_ptr< Shader > >::Factory(pShader);
				}

				const auto pArrayHeapWrapperItem = 0 < indexInput.size() && indexInput[0] ? std::dynamic_pointer_cast<DagValue< std::vector< std::shared_ptr< HeapWrapperItem > > >>(indexInput[0]->GetValue()) : nullptr;
				const auto pArrayConstantBuffer = 1 < indexInput.size() && indexInput[1] ? std::dynamic_pointer_cast<DagValue< std::vector< std::vector< float > > >>(indexInput[1]->GetValue()) : nullptr;

				if (nullptr != pArrayHeapWrapperItem)
				{
					const std::vector< std::shared_ptr< HeapWrapperItem > >& arrayHeapWrapperItem = pArrayHeapWrapperItem->GetRef();
					for (int index = 0; index < (int)arrayHeapWrapperItem.size(); ++index)
					{
						pShader->SetShaderResourceViewHandle(index, arrayHeapWrapperItem[index]);
					}
				}
				if (nullptr != pArrayConstantBuffer)
				{
					const std::vector< std::vector< float > >& arrayConstantBuffer = pArrayConstantBuffer->GetRef();
					for (int index = 0; index < (int)arrayConstantBuffer.size(); ++index)
					{
						pShader->SetConstantBufferData(index, arrayConstantBuffer[index]);
					}
				}

				return pLocalValue;
				});
			return pResult;
		};

		//stackInput 
		// std::shared_ptr< GeometryGeneric >
		//output 
		//  std::vector< std::shared_ptr< GeometryGeneric > >
		mapCalculate["StackGeometry"] = MakeStackFactory<std::shared_ptr< GeometryGeneric >>();

		//indexInput
		//  0.std::shared_ptr< Shader >
		//  1.std::vector< std::shared_ptr< GeometryGeneric > >
		//output 
		//  std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
		mapCalculate["Draw"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< PairData >>(pValue);

				const auto pShader = std::dynamic_pointer_cast<DagValue< std::shared_ptr< Shader > >>(indexInput[0]->GetValue());
				const auto pGeometryList = std::dynamic_pointer_cast<DagValue< std::vector< std::shared_ptr< GeometryGeneric > > >>(indexInput[1]->GetValue());
				PairData newValue(pShader->GetRef(), pGeometryList->GetRef());
				if (nullptr == pLocalValue)
				{
					//pLocalValue = std::make_shared< DagValue< std::vector< std::shared_ptr< Geometry > > > >();
					pLocalValue = DagValue< PairData >::Factory(newValue);
				}
				else
				{
					pLocalValue->SetRef(newValue);
				}
				return pLocalValue;
				});
			return pResult;
		};

		//stackInput
		//  std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
		//output
		//  std::vector< std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > >
		mapCalculate["StackDraw"] = MakeStackFactory<std::pair<std::shared_ptr<Shader>, std::vector<std::shared_ptr<GeometryGeneric>>>>();

		//indexInput
		//  0.DrawSystemFrame
		//  1.IRenderTarget
		//  2.std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
		//output 
		//  IRenderTarget* (same as input 1)
		mapCalculate["RenderList"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
				typedef std::vector< PairData > PairArray;

				const auto pDAGDrawSystemFrame = std::dynamic_pointer_cast<DagValue< DrawSystemFrame* >>(indexInput[0]->GetValue());
				const auto pDAGRenderTarget = std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(indexInput[1]->GetValue());
				//const auto pDAGRenderTargetTexture = std::dynamic_pointer_cast< DagValue< std::shared_ptr<RenderTargetTexture> > >(pValue);
				const auto pDAGDrawList = std::dynamic_pointer_cast<DagValue< PairArray >>(indexInput[2]->GetValue());

				DrawSystemFrame* pDrawSystemFrame = pDAGDrawSystemFrame ? pDAGDrawSystemFrame->Get() : nullptr;
				IRenderTarget* pRenderTarget = pDAGRenderTarget ? pDAGRenderTarget->Get() : nullptr;
				assert(nullptr != pDrawSystemFrame);
				assert(nullptr != pRenderTarget);
				assert(nullptr != pDAGDrawList);
				const auto& drawList = pDAGDrawList->GetRef();

				pDrawSystemFrame->SetRenderTarget(pRenderTarget);

				for (const auto& item : drawList)
				{
					pDrawSystemFrame->SetShader(item.first.get());
					for (const auto& subItem : item.second)
					{
						pDrawSystemFrame->Draw(subItem.get());
					}
				}

				auto pLocalValue = std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< IRenderTarget* >::Factory(nullptr);
				}
				pLocalValue->SetRef(pRenderTarget);

				return pLocalValue;
				});
			return pResult;
		};


		//UI Dag
		//indexInput
		//  0.std::vector<float>:2 ui target size
		//output 
		//  IRenderTarget* 
		mapCalculate["UIDag"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;

				return nullptr;
				});
			return pResult;
		};

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

	m_pDagFrameCount = DagNodeValue::Factory(DagValue<int>::Factory(0));
	m_pDagDrawSystemFrame = DagNodeValue::Factory(
		DagValue<DrawSystemFrame*>::Factory(nullptr),
		false
	);
	m_pDagBackBuffer = DagNodeValue::Factory(DagValue<IRenderTarget*>::Factory(nullptr));
	m_pDagTimeAccumulate = DagNodeValue::Factory(DagValue<float>::Factory(0));

	auto mapValue = FactoryMapValue(m_pDrawSystem.get());
	auto mapCalculate = FactoryMapCalculate(m_pDrawSystem.get(), applicationParam);

	std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues{
		{ "_FrameCount", m_pDagFrameCount },
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

	//hook up Update array
	for (auto& item : jsonData.update)
	{
		auto pTarget = m_pDagCollection->GetDagNode(item.target.name);
		if (nullptr == pTarget)
		{
			continue;
		}

		switch (item.type)
		{
		default:
			break;
		case JSONApplicationDisplayListUpdateType::InputMoveToDagFloatArray:
		{
			JSONApplicationDisplayListSourceMove jsonSourceMove;
			nlohmann::from_json(item.source, jsonSourceMove);
			auto pSource = m_pDagCollection->GetDagNode(jsonSourceMove.name);
			AppendScanCodeArray(jsonSourceMove.scancode);
			m_updateTaskArray.push_back(std::make_shared<UpdateTaskInputMove>(
				pTarget,
				item.target.offset,
				pSource,
				jsonSourceMove,
				m_scanCodeRepeatMap
				));
		}
		break;
		case JSONApplicationDisplayListUpdateType::InputRotateToDagFloatArray:
		{
			JSONApplicationDisplayListSourceRotate jsonSourceRotate;
			nlohmann::from_json(item.source, jsonSourceRotate);
			auto pSource = m_pDagCollection->GetDagNode(jsonSourceRotate.name);
			AppendScanCodeArray(jsonSourceRotate.scancode);
			m_updateTaskArray.push_back(std::make_shared<UpdateTaskInputRotate>(
				pTarget,
				item.target.offset,
				pSource,
				jsonSourceRotate,
				m_scanCodeRepeatMap
				));
		}
		break;
		case JSONApplicationDisplayListUpdateType::SetScreenWidthHeightToDagFloatArray:
			m_updateTaskArray.push_back(std::make_shared< UpdateTaskSetScreenWidthHeight >(
				pTarget,
				item.target.offset,
				m_pDrawSystem.get()
				));
			break;
		}
	}
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

		DagValue<int>::IncrementNode(m_pDagFrameCount, 1);
		DagValue<DrawSystemFrame*>::UpdateNode(m_pDagDrawSystemFrame, pFrame.get());
		DagValue<IRenderTarget*>::UpdateNode(m_pDagBackBuffer, m_pDrawSystem->GetRenderTargetBackBuffer());

		for (auto& item : m_updateTaskArray)
		{
			item->Update(timeDeltaSeconds);
		}

		auto pRender = m_pDagCollection->GetDagNode("_Render");
		if (pRender)
		{
			pRender->GetValue();
		}
	}

	ZeroRepeatScanCodeMap();
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

void ApplicationDisplayList::OnKey(const int vkCode, const int scanCode, const bool repeatFlag, const int repeatCount, bool upFlag)
{
	vkCode; scanCode; repeatFlag; repeatCount; upFlag;
	//LOG_MESSAGE("OnKey %d %d %d %d %d", vkCode, scanCode, repeatFlag, repeatCount, upFlag);

	if (false == upFlag)
	{
		auto found = m_scanCodeRepeatMap.find(scanCode);
		if (found != m_scanCodeRepeatMap.end())
		{
			found->second += repeatCount;
		}
	}

	return;
}

void ApplicationDisplayList::AppendScanCodeArray(const std::vector<int>& scanCodeArray)
{
	for (auto scanCode : scanCodeArray)
	{
		m_scanCodeRepeatMap[scanCode] = 0;
	}
}

void ApplicationDisplayList::ZeroRepeatScanCodeMap()
{
	for (auto& iter : m_scanCodeRepeatMap)
	{
		iter.second = 0;
	}
}
