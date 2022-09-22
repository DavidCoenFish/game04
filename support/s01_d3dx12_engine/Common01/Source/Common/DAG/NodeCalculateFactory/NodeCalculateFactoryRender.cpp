#include "CommonPCH.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryRender.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryDefault.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONShaderResource.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONRenderTarget.h"

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

namespace
{
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
};

void NodeCalculateFactoryRender::Append(std::map<std::string, NodeCalculateFactory>& mapCalculate, const std::filesystem::path& rootPath, DrawSystem* pDrawSystem)
{
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

	/*
	this would almost work, but we should avoid introducing things that can change without ingroming the DAG graph that they change
	ie, the width/height of a renderTaget could change, but how would nodes downstream know.
	*/
#if 0
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
#endif

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
	mapCalculate["StackHeapWrapper"] = NodeCalculateFactoryDefault::MakeStackFactory<std::shared_ptr<HeapWrapperItem>>();

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
			LoadFile(rootPath, jsonShader.vertexShader),
			LoadFile(rootPath, jsonShader.geometryShader),
			LoadFile(rootPath, jsonShader.pixelShader),
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
	mapCalculate["StackGeometry"] = NodeCalculateFactoryDefault::MakeStackFactory<std::shared_ptr< GeometryGeneric >>();

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
	mapCalculate["StackDraw"] = NodeCalculateFactoryDefault::MakeStackFactory<std::pair<std::shared_ptr<Shader>, std::vector<std::shared_ptr<GeometryGeneric>>>>();

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


	//TODO, implement, move to another file?
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

	return;
}
