#include "CommonPCH.h"
#include "Common/Application/ApplicationTriangleJson.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/Json/JSONDrawSystem.h"
#include "Common/Json/JSONGeometry.h"
#include "Common/Json/JSONShader.h"

class JSONData0
{
public:
   JSONDrawSystem drawSystem;
   JSONShader shader; 
   JSONGeometry geometry;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONData0,
   drawSystem,
   shader,
   geometry
   );


IApplication* const ApplicationTriangleJson::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationTriangleJson(hWnd, applicationParam);
}

ApplicationTriangleJson::ApplicationTriangleJson(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
{
   LOG_MESSAGE("ApplicationTriangleJson ctor %p", this);

   JSONData0 data;
   applicationParam.m_json.get_to(data);

   m_pDrawSystem = DrawSystem::Factory(hWnd, data.drawSystem);

   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
   auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.vertexShader);
   auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.pixelShader);
   m_pShader = m_pDrawSystem->MakeShader(
      pCommandList->GetCommandList(),
      data.shader.pipelineState,
      pVertexShaderData,
      nullptr,
      pPixelShaderData
      );
   m_pGeometry = m_pDrawSystem->MakeGeometryGeneric(
      pCommandList->GetCommandList(),
      data.geometry.topology,
      data.geometry.inputElementDesc,
      data.geometry.vertexData,
      data.geometry.floatPerVertex
      );
}

ApplicationTriangleJson ::~ApplicationTriangleJson ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pShader.reset();
   m_pGeometry.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationTriangleJson  dtor %p", this);
}

void ApplicationTriangleJson::Update()
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

void ApplicationTriangleJson::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
