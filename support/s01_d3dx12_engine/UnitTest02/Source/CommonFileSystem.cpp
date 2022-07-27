#include "UnitTestPCH.h"

#include "Common/FileSystem/OverlayFileSystem.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/FileSystem/ProviderMemory.h"
#include "Common/FileSystem/ProviderDisk.h"
#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/FoundStaticFolder.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{
	template<> inline std::wstring ToString<std::vector<uint8_t>>                  (const std::vector<uint8_t>& t)                  
   { 
      std::wstring result(L"std::vector<uint8_t> size:");
      result += std::to_wstring(t.size());
      result += L" [";
      bool first = true;
      for (const auto& item : t)
      {
         if (true == first)
         {
            first = false;
         }
         else
         {
            result += L", ";
         }
         result += std::to_wstring(item);
      }
      result += L" ]";
      return result;
   }
}}}

namespace CommonFileSystem
{
   TEST_CLASS(Sanity)
   {
   public: 
      TEST_METHOD(StaticMethods)
      {
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetTempDir().c_str());
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(L"", FileSystem::GetModualDir(nullptr).c_str());
      }

      TEST_METHOD(BasicOverlayFileSystem)
      {
         ProviderMemory::TStaticFileMap staticMapA = {
               {
                  std::filesystem::path("one.txt"), 
                  std::make_shared<ProviderMemory::TStaticFileData>(ProviderMemory::TStaticFileData({0, std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({1,2,3,4,5}))}))
               }, 
               {
                  std::filesystem::path("two/three.txt"), 
                  std::make_shared<ProviderMemory::TStaticFileData>(ProviderMemory::TStaticFileData({1, std::make_shared<std::vector<uint8_t>>(std::vector<uint8_t>({2,2,3,4,5}))}))
               }, 
            };
         auto pProviderMemoryA = ProviderMemory::Factory(staticMapA);

         std::atomic_bool bFileLoaded = false;
         {
            auto pFileSystem = OverlayFileSystem::Factory({pProviderMemoryA});
            {
               auto pFile = pFileSystem->FindStaticFile("dontexists.txt");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<void*>(nullptr, pFile.get());
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pFile->GetExist());
            }
            {
               auto pFile = pFileSystem->FindStaticFile("one.txt");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<void*>(nullptr, pFile.get());
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pFile->GetExist());
               pFile->AsyncLoadBest([&](const bool error, const std::shared_ptr< std::vector< uint8_t > >& data){
                  Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, error);
                  auto expected = std::vector<uint8_t>({1,2,3,4,5});
                  Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected, *data);
                  bFileLoaded = true;
               });
            }
            {
               auto pFolder = pFileSystem->FindStaticFolder("dontexists");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<void*>(nullptr, pFolder.get());
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(false, pFolder->GetExist());
            }

            {
               auto pFolder = pFileSystem->FindStaticFolder("two");
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual<void*>(nullptr, pFolder.get());
               Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, pFolder->GetExist());
            }
         }
         //the dtor of the file system waits for all the jobs to finish
         Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(true, bFileLoaded.load());
      }

   };
}
