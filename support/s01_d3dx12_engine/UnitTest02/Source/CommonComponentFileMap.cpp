#include "UnitTestPCH.h"

#include "Common/FileSystem/ComponentFileMap.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace CommonComponentFileMap
{
   TEST_CLASS(Basic)
   {
   public: 
      TEST_METHOD(Ctor)
      {
         auto pComponentFileMap = ComponentFileMap<>::Factory(
            ComponentFileMap<>::TMapPathFileData({
               {"one.txt", {}},
               {"two/three.txt", {}},
               })
            );

         Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(pComponentFileMap.get());
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pComponentFileMap->HasFile("four.txt"));
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pComponentFileMap->HasFile("one.txt"));
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pComponentFileMap->HasFolder("four"));
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pComponentFileMap->HasFolder("two"));
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pComponentFileMap->HasFile("two/three.txt"));
      }
   };
}
