#pragma once

#include "Common/Task/ITask.h"
class ApplicationHolder;

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

   TaskWindow(const std::shared_ptr<ApplicationHolder>& pApplicationHolder);
   virtual ~TaskWindow();

   virtual const int Run() override;

private:
   std::shared_ptr<ApplicationHolder> m_pApplicationHolder;

};
