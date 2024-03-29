//
// Main.cpp
//

#include "ApplicationPCH.h"
#include "Build.h"
#include "Common/Application/CommandLine.h"
#include "Common/Task/ITask.h"
#include "Common/Task/TaskWindow.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"
#include "Common/Log/LogConsumerWriteToFile.h"
#include "Common/Util/Utf8.h"
#include "json/json.hpp"

struct JSONApplication
{
   std::vector< JSONTask > tasks;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplication, 
   tasks
   );

static std::map< std::string, TTaskFactory >& GetTaskFactoryMap()
{
   static std::map< std::string, TTaskFactory > s_map(
   {
      {"Window", TaskWindow::Factory}
   });
   return s_map;
}

static const TTaskFactory GetTaskFactory(const std::string& taskFactoryKey)
{
   const auto& factoryMap = GetTaskFactoryMap();
   auto found = factoryMap.find(taskFactoryKey);
   if (found != factoryMap.end())
   {
      return found->second;
   }
   return nullptr;
}

static const int RunTask(HINSTANCE hInstance, int nCmdShow)
{
   auto pLog = Log::Factory(std::vector< std::shared_ptr< ILogConsumer >>({ 
      std::make_shared< LogConsumerConsole >(),
      std::make_shared< LogConsumerWriteToFile >("log.txt")
      }));

   LOG_MESSAGE("Build %s %s %s", Build::GetBuildVersion(), Build::GetBuildTime(), Build::GetBuildDescription());

   auto pCommandLine = CommandLine::Factory(Utf8::Utf16ToUtf8(GetCommandLineW()));
   if (nullptr == pCommandLine)
   {
      return -1;
   }

   int result = 0;
   std::string taskName("Empty");

   if (2 <= pCommandLine->GetParamCount())
   {
      taskName = pCommandLine->GetParam(1);
   }
   else
   {
      LOG_MESSAGE_ERROR("Only got [%d] param, want at least a task name", pCommandLine->GetParamCount());
   }

   {
      std::filesystem::path path = FileSystem::GetModualDir(hInstance) / "Task" / taskName;
      std::filesystem::path applicationPath = path / "Application.json";
      auto fileString = FileSystem::DataToString(FileSystem::SyncReadFile(applicationPath));
      auto json = nlohmann::json::parse( fileString );
      JSONApplication applicationData;
      json.get_to(applicationData);

      for (const auto& item: applicationData.tasks)
      {
         auto taskFactory = GetTaskFactory(item.factory);
         auto pTask = (nullptr != taskFactory) ? taskFactory(hInstance, nCmdShow, pCommandLine, path, item.data) : nullptr;
         if (pTask)
         {
            result = pTask->Run();
         }
         if (result < 0)
         {
            LOG_MESSAGE_ERROR("task returner [%d], abort run", result);
         }
      }
   }

   return result;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   const int result = RunTask(hInstance, nCmdShow);

   return result;
}
