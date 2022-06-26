#include "CommonPCH.h"
#include "Common/Application/ApplicationTextureJson.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/Shader/ShaderResourceInfo.h"
#include "Common/Json/JSONDrawSystem.h"
#include "Common/Json/JSONGeometry.h"
#include "Common/Json/JSONShader.h"
#include "Common/Json/JSONShaderResource.h"
#include "Common/Json/JSONConstantBufferInfo.h"
#include "Common/Json/JSONShaderResourceInfo.h"

class JSONDataATJ
{
public:
   JSONDrawSystem drawSystem;
   JSONShaderResource texture;
   JSONShader shader; 
   JSONGeometry geometry;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDataATJ,
   drawSystem,
   texture,
   shader,
   geometry
   );


IApplication* const ApplicationTextureJson::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationTextureJson(hWnd, applicationParam);
}

ApplicationTextureJson::ApplicationTextureJson(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
{
   LOG_MESSAGE("ApplicationTextureJson ctor %p", this);
   JSONDataATJ data;
   applicationParam.m_json.get_to(data);

   m_pDrawSystem = DrawSystem::Factory(hWnd, data.drawSystem);

   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
   m_pTexture = m_pDrawSystem->MakeShaderResource(
      pCommandList->GetCommandList(),
      m_pDrawSystem->MakeHeapWrapperCbvSrvUav(),
      data.texture.desc,
      data.texture.shaderResourceViewDesc,
      data.texture.data
      );
   auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.vertexShader);
   auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.pixelShader);
   auto arrayTexture = TransformShaderResourceInfo(data.shader.resourceInfo);
   auto arrayConstant = TransformConstantBufferInfo(data.shader.constantInfo);
   
   if (0 < arrayTexture.size())
   {
      arrayTexture[0]->SetShaderResourceViewHandle(m_pTexture->GetHeapWrapperItem());
   }

   m_pShader = m_pDrawSystem->MakeShader(
      pCommandList->GetCommandList(),
      data.shader.pipelineState,
      pVertexShaderData,
      nullptr,
      pPixelShaderData,
      arrayTexture,
      arrayConstant
      );
   m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
      pCommandList->GetCommandList(),
      data.geometry.topology,
      data.geometry.inputElementDesc,
      data.geometry.vertexData,
      data.geometry.floatPerVertex
      );
}

ApplicationTextureJson ::~ApplicationTextureJson ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pTexture.reset();
   m_pShader.reset();
   m_pGeometry.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationTextureJson  dtor %p", this);
}

void ApplicationTextureJson ::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
      pFrame->SetShader(m_pShader.get());
      pFrame->Draw(m_pGeometry.get());
   }
}

void ApplicationTextureJson ::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
