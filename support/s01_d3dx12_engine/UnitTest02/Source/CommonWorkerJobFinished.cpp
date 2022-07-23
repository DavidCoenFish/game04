#include "UnitTestPCH.h"
#include "Common/Worker/JobFinished.h"
#include "Common/Worker/JobFinishedToken.h"
#include "Common/Worker/WorkerCollection.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif


namespace CommonWorkerJobFinished
{
   TEST_CLASS(Basic)
   {
   public: 
      class HoldCount
      {
      public:
         HoldCount()
            : m_count(7)
            , m_bFinished(false)
         {
            LOGGER("HoldCount ctor");
         }
         ~HoldCount()
         {
            LOGGER("HoldCount dtor");
         }
         std::mutex m_mutex;
         int m_count;// = 7;
         bool m_bFinished;// = false;
      };

      TEST_METHOD(CheckWaitNTasks)
      {
         auto pHoldCount = std::make_shared<HoldCount>();
         std::vector< std::function<void()> > workArray;
         {
            auto pJobFinished = JobFinished::Factory([=](){
               std::lock_guard lock(pHoldCount->m_mutex);
               pHoldCount->m_bFinished = true;
            });
            for (int index = 0; index < pHoldCount->m_count; ++index)
            {
               class HoldJob
               {
               public:
                  HoldJob(const std::shared_ptr<JobFinished>& job)
                     : m_job(job)
                  {
                     LOGGER("HoldJob ctor");
                  }
                  ~HoldJob()
                  {
                     LOGGER("HoldJob dtor");
                  }
                  std::shared_ptr<JobFinished> m_job;
               };
               auto pHoldJob = std::make_shared<HoldJob>(pJobFinished);
               //workArray.push_back([&count,&mutex,=pJobFinished](){
               std::shared_ptr<JobFinished> copyJobFinished(pJobFinished);
               workArray.push_back([=](){
                  {
                     std::lock_guard lock(pHoldCount->m_mutex);
                     pHoldCount->m_count -= 1;
                  }
                  pHoldJob->m_job = nullptr;
               });
            }
         }

         LOGGER("pre check 0");
         {
            std::lock_guard lock(pHoldCount->m_mutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pHoldCount->m_bFinished);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(7, pHoldCount->m_count);
         }

         LOGGER("pre workerCollection");
         {
            WorkerCollection< 3 > workerCollection;
            workerCollection.AddWork(workArray);
         }

         LOGGER("pre check 1");
         {
            std::lock_guard lock(pHoldCount->m_mutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pHoldCount->m_bFinished);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, pHoldCount->m_count);
         }

         LOGGER("end");
      }

      TEST_METHOD(CheckWaitNTasksSkiny)
      {
         auto pHoldCount = std::make_shared<HoldCount>();
         std::vector< std::function<void()> > workArray;
         {
            auto pJobFinished = JobFinished::Factory([=](){
               std::lock_guard lock(pHoldCount->m_mutex);
               pHoldCount->m_bFinished = true;
            });
            for (int index = 0; index < pHoldCount->m_count; ++index)
            {
               auto pJobToken = pJobFinished->MakeToken();
               workArray.push_back([=](){
                  {
                     std::lock_guard lock(pHoldCount->m_mutex);
                     pHoldCount->m_count -= 1;
                  }
                  pJobToken->Clear();
               });
            }
         }

         LOGGER("pre check 0");
         {
            std::lock_guard lock(pHoldCount->m_mutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pHoldCount->m_bFinished);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(7, pHoldCount->m_count);
         }

         LOGGER("pre workerCollection");
         {
            WorkerCollection< 3 > workerCollection;
            workerCollection.AddWork(workArray);
         }

         LOGGER("pre check 1");
         {
            std::lock_guard lock(pHoldCount->m_mutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pHoldCount->m_bFinished);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, pHoldCount->m_count);
         }

         LOGGER("end");
      }

   };
}
