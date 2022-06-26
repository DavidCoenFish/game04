#include "CommonPCH.h"
#include "Common/Application/ApplicationTextureMove.h"
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

class JSONDataTextureMove
{
public:
   JSONDrawSystem drawSystem;
   JSONShaderResource texture;
   JSONShader shader; 
   JSONGeometry geometry;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDataTextureMove,
   drawSystem,
   texture,
   shader,
   geometry
   );

struct VertexConstants
{
   float m_time;
   float padding0; 
   float padding1; 
   float padding2; 
};

IApplication* const ApplicationTextureMove::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationTextureMove(hWnd, applicationParam);
}

ApplicationTextureMove::ApplicationTextureMove(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
   , m_timePointValid(false)
   , m_timePoint()
   , m_timeAccumulation(0.0f)
{
   LOG_MESSAGE("ApplicationTextureMove ctor %p", this);

   JSONDataTextureMove data;
   applicationParam.m_json.get_to(data);

   m_pDrawSystem = DrawSystem::Factory(hWnd, data.drawSystem);

   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
   auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.vertexShader);
   auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / data.shader.pixelShader);

   auto arrayTexture = TransformShaderResourceInfo(data.shader.resourceInfo);
   auto arrayConstant = TransformConstantBufferInfo(data.shader.constantInfo);
   
   m_pTexture = m_pDrawSystem->MakeShaderResource(
      pCommandList->GetCommandList(),
      m_pDrawSystem->MakeHeapWrapperCbvSrvUav(),
      data.texture.desc,
      data.texture.shaderResourceViewDesc,
      data.texture.data
      );

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

ApplicationTextureMove ::~ApplicationTextureMove ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pShader.reset();
   m_pGeometry.reset();
   m_pTexture.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationTextureMove  dtor %p", this);
}

void ApplicationTextureMove::Update()
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
   m_timeAccumulation = fmod(m_timeAccumulation + (timeDeltaSeconds * 0.25f), 1.0f);
   if (nullptr != m_pShader)
   {
      m_pShader->GetConstant<VertexConstants>(0).m_time = m_timeAccumulation;
   }

   if (nullptr != m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());
      pFrame->SetShader(m_pShader.get());
      pFrame->Draw(m_pGeometry.get());
   }
}

void ApplicationTextureMove::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
