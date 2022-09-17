#pragma once

#include "json/json.hpp"

class CommandLine;
class TaskWindow;

class IApplicationParam
{
public:
   IApplicationParam(
      const bool bFullScreen,
      const int width,
      const int height,
      const std::shared_ptr< CommandLine >& pCommandLine,
      const std::filesystem::path& rootPath, 
      const nlohmann::json& json,
      TaskWindow* const pTaskWindow
   );
   const bool  m_bFullScreen;
   const int  m_width;
   const int  m_height;
   const std::shared_ptr< CommandLine >m_pCommandLine;
   const std::filesystem::path m_rootPath;
   const nlohmann::json m_json;
   TaskWindow* const m_pTaskWindow;
};

class IApplication
{
public:
   IApplication(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~IApplication();

   virtual void Update();
   virtual void OnWindowMoved();
   virtual void OnWindowSizeChanged(const int width, const int height);
   virtual void OnActivated();
   virtual void OnDeactivated();
   virtual void OnSuspending();
   virtual void OnResuming();
   virtual void OnKey(const int vkCode, const int scanCode, const bool repeatFlag, const int repeatCount, bool upFlag);

   const int GetDefaultWidth() const { return m_defaultWidth; }
   const int GetDefaultHeight() const { return m_defaultHeight; }

   const bool GetInSizemove() const { return m_bInSizemove; }
   void SetInSizemove(const bool bInSizemove) { m_bInSizemove = bInSizemove; }

   const bool GetMinimized() const { return m_bMinimized; }
   void SetMinimized(const bool bMinimized) { m_bMinimized = bMinimized; }

   const bool GetFullScreen() const { return m_bFullScreen; }
   void SetFullScreen(const bool bFullScreen) { m_bFullScreen = bFullScreen; }

   void Destroy(const int exitCode);

private:
   HWND m_hWnd; 
   int m_defaultWidth;
   int m_defaultHeight;

   bool m_bInSizemove;
   bool m_bInSuspend;
   bool m_bMinimized;
   bool m_bFullScreen;

   TaskWindow* const m_pTaskWindow;

};