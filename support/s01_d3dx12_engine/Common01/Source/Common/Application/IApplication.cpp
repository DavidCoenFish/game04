#include "CommonPCH.h"
#include "Common/Application/IApplication.h"
#include "Common/Log/Log.h"
#include "Common/Task/TaskWindow.h"

IApplicationParam::IApplicationParam(
   const bool bFullScreen,
   const int width,
   const int height,
   const std::shared_ptr< CommandLine >& pCommandLine,
   const std::filesystem::path& rootPath, 
   const nlohmann::json& json,
   TaskWindow* const pTaskWindow
   )
   : m_bFullScreen(bFullScreen)
   , m_width(width)
   , m_height(height)
   , m_pCommandLine(pCommandLine)
   , m_rootPath(rootPath)
   , m_json(json)
   , m_pTaskWindow(pTaskWindow) 
{
   //nop
}

IApplication::IApplication(const HWND hWnd, const IApplicationParam& applicationParam)
   : m_hWnd(hWnd)
   , m_bInSizemove(false)
   , m_bInSuspend(false)
   , m_bMinimized(false)
   , m_bFullScreen(applicationParam.m_bFullScreen)
   , m_defaultWidth(applicationParam.m_width)
   , m_defaultHeight(applicationParam.m_height)
   , m_pTaskWindow(applicationParam.m_pTaskWindow)
{
   LOG_MESSAGE("IApplication ctor %p", this);
}

IApplication::~IApplication()
{
   LOG_MESSAGE("IApplication dtor %p", this);
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

void IApplication::OnKey(const int, const int, const bool, const int, bool)
{
   return;
}

void IApplication::Destroy(const int exitCode)
{
	if (nullptr != m_pTaskWindow)
	{
		m_pTaskWindow->DestroyApplication(this, exitCode);
	}
	return;
}

