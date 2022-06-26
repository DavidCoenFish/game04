#include "UnitTestPCH.h"

#include "Common/Util/Utf8.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace UnitTestGeneral
{
   TEST_CLASS(LanguageFeatures)
   {
   public:
 
      TEST_METHOD(VectorConstructionLiteral)
      {
         std::vector< int > data({0,1,2,3,4});
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<size_t>(5, data.size() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, data[0] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, data[1] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, data[2] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(3, data[3] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(4, data[4] );
      }

      TEST_METHOD(VectorConstructionLiteralPointer)
      {
         std::vector< void* > data({nullptr,nullptr,nullptr,nullptr});
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<size_t>(4, data.size() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[0] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[1] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[2] );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual<void*>(nullptr, data[3] );
      }
   };

   TEST_CLASS(Signal)
   {
      TEST_METHOD(CanYouSetEventBeforeTheWait)
      {
         auto localEvent = CreateEvent( 
            NULL,   // default security attributes
            FALSE,  // auto-reset event object
            FALSE,  // initial state is nonsignaled
            NULL);  // unnamed object
         if (0 != localEvent)
         {
            SetEvent(localEvent);
            WaitForSingleObject(localEvent, INFINITE);
            CloseHandle(localEvent);
         }
      }

      TEST_METHOD(CanYouConstructEventBeforeTheWait)
      {
         auto localEvent = CreateEvent( 
            NULL,   // default security attributes
            FALSE,  // auto-reset event object
            TRUE,  // initial state is signaled
            NULL);  // unnamed object
         if (0 != localEvent)
         {
            WaitForSingleObject(localEvent, INFINITE);
            CloseHandle(localEvent);
         }
      }
   };

   TEST_CLASS(FileSystem)
   {
   public:
      TEST_METHOD(FileSystemPathCmp)
      {
         std::filesystem::path path00("one.txt");
         std::filesystem::path path01("two.txt");
         std::filesystem::path path02("two.txt");
         std::filesystem::path path03("");

         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, path00 == path01);
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, path01 == path02);
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, path00 == path03);

      }

   };
}
