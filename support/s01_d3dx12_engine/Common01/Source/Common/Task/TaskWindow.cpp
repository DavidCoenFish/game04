#include "CommonPCH.h"

#include "Common/Task/TaskWindow.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/ApplicationComputeShader.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/Application/ApplicationTexture.h"
#include "Common/Application/ApplicationTriangle.h"
#include "Common/Application/IApplication.h"

class JSONWindow
{
public:
   JSONWindow()
      : fullScreen(false)
      , width(800)
      , height(600)
   {
      //nop
   }
   std::string name;
   bool fullScreen;
   int width;
   int height;
   std::string factory;
   nlohmann::json data;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONWindow,
   name,
   fullScreen,
   width,
   height,
   factory,
   data
   );

//works if we have live code referencing this class? at which point just as easy to have awareness of headers?
//extern void RegisterTaskFactory(const std::string& taskFactoryKey, const TTaskFactory& taskFactory);
//class CallThing
//{
//public:
//   CallThing()
//   {
//      RegisterTaskFactory("Window", [](
//         const HINSTANCE hInstance, 
//         const int nCmdShow, 
//         const std::shared_ptr< CommandLine >& pCommandLine, 
//         const std::filesystem::path& path, 
//         const nlohmann::json& json
//         ){
//         //return std::make_shared<TaskWindow>(hInstance, nCmdShow, pCommandLine, path, json);
//         return TaskWindow::Factory(hInstance, nCmdShow, pCommandLine, path, json);
//      });
//   }
//};
//static CallThing s_CallThing;

const TApplicationFactory GetApplicationFactory(const std::string& factory)
{
   static std::map<std::string, TApplicationFactory> s_factoryMap({
      {"ComputeShader", ApplicationComputeShader::Factory},
      {"DisplayList", ApplicationDisplayList::Factory},
      {"Texture", ApplicationTexture::Factory},
      {"Triangle", ApplicationTriangle::Factory}
      });
   const auto found = s_factoryMap.find(factory);
   if (found != s_factoryMap.end())
   {
      return found->second;
   }
   return [](const HWND hWnd, const IApplicationParam& param)
   {
       return new IApplication(hWnd, param);
   };
}

const std::shared_ptr<TaskWindow> TaskWindow::Factory(
   const HINSTANCE hInstance, 
   const int nCmdShow, 
   const std::shared_ptr< CommandLine >& pCommandLine, 
   const std::filesystem::path& path, 
   const nlohmann::json& json
   )  
{
   std::vector<JSONWindow> arrayWindow;
   json.get_to(arrayWindow);

   auto pResult = std::make_shared<TaskWindow>();
   TaskWindow* pRawTaskWindow = pResult.get();
   for (const auto& item : arrayWindow)
   {
      auto applicationParam = IApplicationParam(
         item.fullScreen,
         item.width,
         item.height,
         pCommandLine,
         path,
         item.data,
         pRawTaskWindow
         //[=](IApplication* pApplication, const int exitCode){
         //     pRawTaskWindow->DestroyApplication(pApplication, exitCode);
         // }
         );

      auto applicationFactory = GetApplicationFactory(item.factory);
      auto applicationFactoryWrapped = [pRawTaskWindow,applicationFactory](const HWND hWnd, const IApplicationParam& param)
      {
          IApplication* pApplication = applicationFactory(hWnd, param);
          std::shared_ptr< IApplication > pApplicationShared(pApplication);
          pRawTaskWindow->m_pApplicationList.push_back(pApplicationShared);
          return pApplication;
      };

      WindowHelper(
         applicationFactoryWrapped,
         applicationParam,
         hInstance,
         item.name,
         nCmdShow
         );
   }

   return pResult;
}

TaskWindow::TaskWindow()
    : m_bContinue(true)
{
   return;
}

TaskWindow::~TaskWindow()
{
   return;
}

const int TaskWindow::Run()
{
   //while we have windows, keep pushing messages
   MSG msg = {};
   int exitCode = 0;
   while (true == m_bContinue)
   {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
         if (WM_QUIT == msg.message)
         {
             exitCode = (int)msg.wParam;
             break;
         }
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      else
      {
          for (auto iter: m_pApplicationList)
          {
              iter->Update();
          }
      }
   }

   return exitCode;
}

void TaskWindow::DestroyApplication(IApplication* pApplication, const int exitCode)
{
    m_pApplicationList.erase(
        std::remove_if(m_pApplicationList.begin(), m_pApplicationList.end(), [=](std::shared_ptr< IApplication >& item){
        return (pApplication == item.get());
        }),
        m_pApplicationList.end());
    if (0 == m_pApplicationList.size())
    {
        PostQuitMessage(exitCode);
    }
    return;
}
