#include "UnitTestPCH.h"
#include "Common/Worker/WorkerTask.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif


namespace CommonWorkerTask
{
   TEST_CLASS(Basic)
   {

   public:
 
      TEST_METHOD(DoesDtorWaitForTask)
      {
         std::mutex countMutex;
         int count = 0;
         {
            auto pWorkerTask = WorkerTask::Factory([&]()
            {
               std::this_thread::sleep_for(std::chrono::milliseconds(100));
               //std::this_thread::sleep_for(std::chrono::seconds(10));
               {
                  std::lock_guard< std::mutex > lock(countMutex);
                  count += 1;
               }
            });
            pWorkerTask->SignalWorkToDo();
         }
         {
            std::lock_guard< std::mutex > lock(countMutex);
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, count );
         }
      }

#if 1
      TEST_METHOD(DoesDtorWaitForTask100)
      {
         for (int index = 0; index < 100; ++index)
         {
            LOGGER(std::to_string(index).c_str());

            std::mutex countMutex;
            int count = 0;
            {
               auto pWorkerTask = WorkerTask::Factory([&]()
               {
                  std::lock_guard< std::mutex > lock(countMutex);
                  count += 1;
               });
               pWorkerTask->SignalWorkToDo();
            }
            {
               std::lock_guard< std::mutex > lock(countMutex);
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, count );
            }
         }
      }
#endif

   };
}
