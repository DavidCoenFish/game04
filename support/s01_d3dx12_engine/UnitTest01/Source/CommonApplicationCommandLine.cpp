#include "UnitTestPCH.h"

#include "Common/Application/CommandLine.h"
#include "Common/Log/Log.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonApplicationCommandLine
{
   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(Empty)
      {
         auto pCommandLine = CommandLine::Factory("");
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pCommandLine.get() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, pCommandLine->GetParamCount() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("", pCommandLine->GetParam(0).c_str() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pCommandLine->GetFlag("Empty") );
         Log::RunningUintTest();
      }
      TEST_METHOD(Simple)
      {
         auto pCommandLine = CommandLine::Factory("Empty");
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pCommandLine.get() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, pCommandLine->GetParamCount() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("Empty", pCommandLine->GetParam(0).c_str() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pCommandLine->GetFlag("Empty") );
         Log::RunningUintTest();
      }
      TEST_METHOD(Normal)
      {
         auto pCommandLine = CommandLine::Factory("Empty bar=foo \"and something in quotes\"");
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pCommandLine.get() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(3, pCommandLine->GetParamCount() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("Empty", pCommandLine->GetParam(0).c_str() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("and something in quotes", pCommandLine->GetParam(2).c_str() );
         std::string value;
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pCommandLine->GetValueString("bar", value) );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("foo", value.c_str() );
         Log::RunningUintTest();
      }
      TEST_METHOD(Numeric)
      {
         auto pCommandLine = CommandLine::Factory("a=10 b=-655360 c=0");
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pCommandLine.get() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(3, pCommandLine->GetParamCount() );
         {
            int value;
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pCommandLine->GetValueInt("a", value) );
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(10, value);
         }
         {
            int value;
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pCommandLine->GetValueInt("b", value) );
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(-655360, value);
         }
         {
            int value;
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pCommandLine->GetValueInt("c", value) );
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(0, value);
         }
         {
            int value;
            Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pCommandLine->GetValueInt("d", value) );
         }
         Log::RunningUintTest();
      }
      TEST_METHOD(Quotes)
      {
         auto pCommandLine = CommandLine::Factory("\"\"\"hello\"\" \"\"world\"\"\" \"two\"");
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pCommandLine.get() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, pCommandLine->GetParamCount() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("\"hello\" \"world\"", pCommandLine->GetParam(0).c_str() );
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual("two", pCommandLine->GetParam(1).c_str() );
         Log::RunningUintTest();
      }
   };
}
