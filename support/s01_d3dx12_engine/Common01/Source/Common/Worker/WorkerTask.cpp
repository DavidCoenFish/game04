#include "CommonPCH.h"

#include "Common/Worker/WorkerTask.h"

std::shared_ptr<WorkerTask> WorkerTask::Factory(const std::function<void(void)>& task)
{
   return std::make_shared<WorkerTask>(task);
}

WorkerTask::WorkerTask(const std::function<void(void)>& task) //const std::future<_Ty>& future)
: m_task(task)
, m_event(0)
, m_flagWork(0)
, m_flagDtor(false)
{
   m_event = CreateEvent( 
      NULL,   // default security attributes
      FALSE,  // auto-reset event object
      FALSE,  // initial state is nonsignaled
      NULL);  // unnamed object

   m_future = std::async(std::launch::async, [=](){
      DoWork();
   });
   return;
}

WorkerTask::~WorkerTask()
{
   m_flagDtor = true;
   if (nullptr != m_event)
   {
      SetEvent(m_event);
   }
   if (true == m_future.valid())
   {
      m_future.wait();
   }
   if (nullptr != m_event)
   {
      CloseHandle(m_event);
   }
   m_event = nullptr;
   return;
}

void WorkerTask::SignalWorkToDo()
{
   m_flagWork++;
   SetEvent(m_event);
   return;
}

void WorkerTask::DoWork()
{
   int flagWorkCache = 0;
   while (true)
   {
      WaitForSingleObject(m_event, INFINITE);

      const int flagWork = m_flagWork.load();
      if (flagWorkCache != flagWork)
      {
         flagWorkCache = flagWork;
         //we expect task to keep running while it has work, rather than telling to run once for each time SignalWorkToDo() has been called
         m_task();
      }

      if (true == m_flagDtor)
      {
         break;
      }
   }
   return;
}
