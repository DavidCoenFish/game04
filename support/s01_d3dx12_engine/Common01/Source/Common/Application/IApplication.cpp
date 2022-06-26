#include "CommonPCH.h"
#include "Common/Application/IApplication.h"
#include "Common/Application/ApplicationHolder.h"
#include "Common/Log/Log.h"

IApplicationParam::IApplicationParam(
   const std::shared_ptr<ApplicationHolder>& pApplicationHolder, 
   const bool bFullScreen,
   const int width,
   const int height,
   const std::shared_ptr< CommandLine >& pCommandLine,
   const std::filesystem::path& rootPath, 
   const nlohmann::json& json
   )
   : m_pApplicationHolder(pApplicationHolder)
   , m_bFullScreen(bFullScreen)
   , m_width(width)
   , m_height(height)
   , m_pCommandLine(pCommandLine)
   , m_rootPath(rootPath)
   , m_json(json)
{
   //nop
}

IApplication::IApplication(const HWND hWnd, const IApplicationParam& applicationParam)
   : m_hWnd(hWnd)
   , m_pTaskHolder(applicationParam.m_pApplicationHolder)
   , m_bInSizemove(false)
   , m_bInSuspend(false)
   , m_bMinimized(false)
   , m_bFullScreen(applicationParam.m_bFullScreen)
   , m_defaultWidth(applicationParam.m_width)
   , m_defaultHeight(applicationParam.m_height)
{
   LOG_MESSAGE("IApplication ctor %p", this);
   if (nullptr != m_pTaskHolder)
   {
      m_pTaskHolder->AddApplication(this);
   }
}

IApplication::~IApplication()
{
   LOG_MESSAGE("IApplication dtor %p", this);

   if (nullptr != m_pTaskHolder)
   {
      m_pTaskHolder->RemoveApplication(this);
   }
}

void IApplication::Update()
{
   return;
}

void IApplication::OnWindowMoved()
{
   return;
}

void IApplication::OnWindowSizeChanged(const int, const int)
{
   return;
}

void IApplication::OnActivated()
{
   return;
}

void IApplication::OnDeactivated()
{
   return;
}

void IApplication::OnSuspending()
{
   return;
}

void IApplication::OnResuming()
{
   return;
}

