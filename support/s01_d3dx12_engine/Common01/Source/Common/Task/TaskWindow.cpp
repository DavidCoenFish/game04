#include "CommonPCH.h"

#include "Common/Task/TaskWindow.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/ApplicationHolder.h"
#include "Common/Application/ApplicationComputeShader.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/Application/ApplicationText.h"
#include "Common/Application/ApplicationTexture.h"
#include "Common/Application/ApplicationTextureJson.h"
#include "Common/Application/ApplicationTextureMove.h"
#include "Common/Application/ApplicationTriangle.h"
#include "Common/Application/ApplicationTriangleJson.h"
#include "Common/Application/ApplicationTriangleMove.h"
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
      {"Text", ApplicationText::Factory},
      {"Texture", ApplicationTexture::Factory},
      {"TextureJson", ApplicationTextureJson::Factory},
      {"TextureMove", ApplicationTextureMove::Factory},
      {"Triangle", ApplicationTriangle::Factory},
      {"TriangleJson", ApplicationTriangleJson::Factory},
      {"TriangleMove", ApplicationTriangleMove::Factory},
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

   auto pApplicationHolder = std::make_shared<ApplicationHolder>();
   for (const auto& item : arrayWindow)
   {
      auto applicationParam = IApplicationParam(
         pApplicationHolder, 
         item.fullScreen,
         item.width,
         item.height,
         pCommandLine,
         path,
         item.data
         );

      auto applicationFactory = GetApplicationFactory(item.factory);
      WindowHelper(
         applicationFactory,
         applicationParam,
         hInstance,
         item.name,
         nCmdShow
         );
   }

   auto pResult = std::make_shared<TaskWindow>(pApplicationHolder); 
   return pResult;
}

TaskWindow::TaskWindow(const std::shared_ptr<ApplicationHolder>& pApplicationHolder)
   : m_pApplicationHolder(pApplicationHolder)
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
   while (true == m_pApplicationHolder->HasApplication())
   {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      else
      {
         m_pApplicationHolder->Update();
      }
   }

   return 0;
}
