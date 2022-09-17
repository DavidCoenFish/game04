#pragma once

#include "Common/Task/ITask.h"
class IApplication;

class TaskWindow : public ITask
{
public:
   static const std::shared_ptr<TaskWindow> Factory(
      const HINSTANCE hInstance, 
      const int nCmdShow, 
      const std::shared_ptr< CommandLine >& pCommandLine, 
      const std::filesystem::path& path, 
      const nlohmann::json& json
      );

   TaskWindow();
   virtual ~TaskWindow();

   virtual const int Run() override;

   //void RegisterApplication(IApplication* const pApplication);
   void DestroyApplication(IApplication* pApplication, const int exitCode);

private:
   //is the mutex redundant? message loop must be on main thread?
   //mutable std::mutex m_pApplicationMutex;
   //IApplication* m_pApplication;
   //std::unique_ptr< IApplication > m_pApplication;
    std::vector< std::shared_ptr< IApplication > > m_pApplicationList;
    bool m_bContinue;
};
