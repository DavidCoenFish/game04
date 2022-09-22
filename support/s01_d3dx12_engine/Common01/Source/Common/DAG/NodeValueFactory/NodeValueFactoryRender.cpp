#include "CommonPCH.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryRender.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONShaderResource.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONRenderTarget.h"

void NodeValueFactoryRender::Append(std::map<std::string, NodeValueFactory>& mapValue, DrawSystem* pDrawSystem)
{
	mapValue["Geometry"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		JSONGeometry jsonGeometry;
		data.get_to(jsonGeometry);
		auto pCommandList = pDrawSystem->CreateCustomCommandList();
		auto pGeometry = pDrawSystem->MakeGeometryGeneric(
			pCommandList->GetCommandList(),
			jsonGeometry.topology,
			jsonGeometry.inputElementDesc,
			jsonGeometry.vertexData,
			jsonGeometry.floatPerVertex
		);
		auto pValue = DagValue<std::shared_ptr< GeometryGeneric >>::Factory(pGeometry);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};

	//move render target to calculate and take input of width and height?
	mapValue["RenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		JSONRenderTarget jsonRenderTarget;
		data.get_to(jsonRenderTarget);
		auto pCommandList = pDrawSystem->CreateCustomCommandList();
		auto pRenderTargetTexture = pDrawSystem->MakeRenderTargetTexture(
			pCommandList->GetCommandList(),
			jsonRenderTarget.targetFormatDataArray,
			jsonRenderTarget.targetDepthData,
			jsonRenderTarget.width,
			jsonRenderTarget.height,
			jsonRenderTarget.resizeWithScreen
		);
		auto pValue = DagValue<std::shared_ptr< RenderTargetTexture >>::Factory(pRenderTargetTexture);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};
	mapValue["Texture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		JSONShaderResource jsonShaderResource;
		data.get_to(jsonShaderResource);
		auto pCommandList = pDrawSystem->CreateCustomCommandList();
		auto pShaderResource = pDrawSystem->MakeShaderResource(
			pCommandList->GetCommandList(),
			pDrawSystem->MakeHeapWrapperCbvSrvUav(),
			jsonShaderResource.desc,
			jsonShaderResource.shaderResourceViewDesc,
			jsonShaderResource.data
		);
		auto pValue = DagValue<std::shared_ptr< ShaderResource >>::Factory(pShaderResource);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};

	return;
}
