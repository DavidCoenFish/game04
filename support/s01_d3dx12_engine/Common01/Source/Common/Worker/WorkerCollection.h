#pragma once

/*
wanted a collection of worker threads that i could give a set of tasks to

add a template param of finish all work before shutdown? current setup will try to finish all tasks before shutdown?
could add a remove tasks to clear any unfinished work? "ClearWork" to allow early abort on shutdown
*/
#include "Common/Worker/WorkerTask.h"

template <int _ThreadCount> //, bool _finishAllWorkBeforeShutdown = false>
class WorkerCollection
{
public:
   static std::shared_ptr<WorkerCollection> Factory()
   {
      return std::make_shared<WorkerCollection>();
   }

   WorkerCollection()
      : m_workingThreadCount(0)
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index] = std::make_shared<WorkerTask>([=](){DoWork();});
      }
      return;
   }

   ~WorkerCollection()
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index] = nullptr;
      }
      return;
   }
   void AddWork(const std::vector< std::function<void()> >& workArray)
   {
      {
         std::lock_guard< std::mutex > lock(m_workArrayMutex);
         m_workArray.insert( m_workArray.end(), workArray.begin(), workArray.end() );
      }
      SignalWork();

      return;
   }

   void AddWork(const std::function<void()>& work)
   {
      {
         std::lock_guard< std::mutex > lock(m_workArrayMutex);
         m_workArray.push_back(work);
      }
      SignalWork();

      return;
   }

   // delete any incomplete work, allows quicker shutdown if called, as on dtor we try to finished everything first...
   void ClearWork()
   {
      std::lock_guard< std::mutex > lock(m_workArrayMutex);
      m_workArray.clear();
   }

   void SetActiveWorkFinishedCallback(const std::function<void()>& callbackActiveWorkFinished)
   {
      std::lock_guard< std::mutex > lock(m_callbackActiveWorkFinishedMutex);
      m_callbackActiveWorkFinished.push_back(callbackActiveWorkFinished);
      return;
   }

private:
   void SignalWork()
   {
      for (int index = 0; index < _ThreadCount; ++index)
      {
         m_workerThread[index]->SignalWorkToDo();
      }
   }

   void DoWork()
   {
      std::function<void()> work;
      while (true)
      {
         {
            std::lock_guard< std::mutex > lock(m_workArrayMutex);
            if (false == m_workArray.empty())
            {
               work = m_workArray.front();
               m_workArray.pop_front();
               m_workingThreadCount += 1;
            }
         }

         if (nullptr == work)
         {
            return;
         }

         work();

         //if we finished work, and we are the last active worker, consume the ActiveWorkFinished
         bool bLastToFinish = false;
         {
            std::lock_guard< std::mutex > lock(m_workArrayMutex);
            if (nullptr != work)
            {
               m_workingThreadCount -= 1;

               if ((true == m_workArray.empty()) && (m_workingThreadCount == 0))
               {
                  bLastToFinish = true;
               }
            }
         }
         if (true == bLastToFinish)
         {
            DoActiveWorkFinishedCallback();
         }

         work = nullptr;
      }
      //if there is any more on the array...? otherwise there is a risk of items never removed from list? use while loop instead
      //SignalWorkToDo();
   }

   void DoActiveWorkFinishedCallback()
   {
      std::vector< std::function<void()> > localCopy;
      {
         std::lock_guard< std::mutex > lock(m_callbackActiveWorkFinishedMutex);
         localCopy = m_callbackActiveWorkFinished;
         m_callbackActiveWorkFinished.clear();
      }
      for (auto iter: localCopy)
      {
         iter();
      }
   }

private:
   std::shared_ptr<WorkerTask> m_workerThread[_ThreadCount];

   std::mutex m_workArrayMutex;
   std::list< std::function<void(void)> > m_workArray;
   int m_workingThreadCount; //we could remove m_workingThreadCount if m_callbackActiveWorkFinished is empty, but would still need m_workArrayMutex's lock for m_workArray

   std::mutex m_callbackActiveWorkFinishedMutex;
   std::vector< std::function<void()> > m_callbackActiveWorkFinished;
};
