#include "UnitTestPCH.h"

#include "Common/Worker/WorkerCollection.h"
#include "Common/Log/Log.h"
#include "Common/Log/LogConsumerConsole.h"

/*
cls & "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" G:\dcoen\SolutionBuild\UnitTest01\x64\Debug\output\UnitTest01.dll
*/

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
   #define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
   #define LOGGER(message) (void)0
#endif

#define LOGGER_ALLWAYS(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)

namespace CommonWorkerCollection
{
   TEST_CLASS(Basic)
   {

   public:
 
      TEST_METHOD(DoesWorkerCollectionCallFinishedCallback)
      {
         for (int index = 0; index < 100; ++index)
         {
            //Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(std::to_string(index).c_str());

            auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char str[32];
            ctime_s(str,sizeof str,&localTime);
            LOGGER(&str[0]);

            std::mutex mutex;
            bool finishedCallbackA = false;
            bool finishedCallbackB = false;
            int workCount = 0;
            auto task = [&]()
            {
               std::lock_guard lock(mutex);
               LOGGER("task");
               workCount += 1;
            };
            auto callbackA = [&]()
            {
               std::lock_guard lock(mutex);
               LOGGER("callbackA");
               finishedCallbackA = true;
            };
            auto callbackB = [&]()
            {
               std::lock_guard lock(mutex);
               LOGGER("callbackB");
               finishedCallbackB = true;
            };

            {
               WorkerCollection< 3 > workerCollection;
               workerCollection.SetActiveWorkFinishedCallback(callbackA);
               workerCollection.SetActiveWorkFinishedCallback(callbackB);
               LOGGER("pre add work");
               workerCollection.AddWork(std::vector<std::function<void(void)>>({task, task, task, task, task}));
               LOGGER("post add work");
            }

            {
               LOGGER("end0");
               std::lock_guard lock(mutex);
               LOGGER("end1");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, finishedCallbackA);
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, finishedCallbackB);
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(5, workCount);
            }
         }
      }

   };
}
