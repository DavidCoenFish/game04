#include "CommonPCH.h"
#include "Common/Application/ApplicationMeshShader.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"

IApplication* const ApplicationMeshShader::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationMeshShader(hWnd, applicationParam);
}

ApplicationMeshShader::ApplicationMeshShader(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
{
   LOG_MESSAGE("ApplicationMeshShader ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(hWnd);
}

ApplicationMeshShader::~ApplicationMeshShader ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationMeshShader dtor %p", this);
}

void ApplicationMeshShader::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();
   }
}

void ApplicationMeshShader::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
