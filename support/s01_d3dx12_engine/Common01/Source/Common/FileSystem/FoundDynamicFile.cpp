#include "CommonPCH.h"

#include "Common/FileSystem/FoundDynamicFile.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

static void LoadDynamicFile(
   const std::shared_ptr< IFileSystemVisitorFound >& pFileSystem, 
   const FoundDynamicFile::TLoadCallback& loadCallback,
   const std::filesystem::path& path,
   IFileSystemProvider* pBestProvider
   )
{
   if ((nullptr != pBestProvider) && (nullptr != pFileSystem))
   {
      pFileSystem->AddAsyncTask([=](){
         pBestProvider->AsyncLoadDynamicFile(loadCallback, path);
      });
   }
}

std::shared_ptr< FoundDynamicFile > FoundDynamicFile::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
{
   return std::make_shared<FoundDynamicFile>(
      path, 
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundDynamicFile::FoundDynamicFile(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
   : m_path(path)
   , m_filter(filter)
   , m_arrayProvider(arrayProviders)
   , m_pFileSystem(pFileSystem)
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->AddFoundDynamicFile(this);
      }
   }
}

FoundDynamicFile::~FoundDynamicFile()
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->RemoveFoundDynamicFile(this);
      }
   }
}

const int FoundDynamicFile::GetFilter() const
{
   return m_filter;
}
void FoundDynamicFile::OnProviderChange(IFileSystemProvider* const)
{
   //waiting on adding m_arrayCallbackBestChange, m_arrayCallbackAllChange
   return;
}

//when the function was called, the file existed
// we dont cache found/exists, as we have a AsyncSaveAll, AsyncDeleteAll, 
// which over a period of time, can change files against multiple providers
const bool FoundDynamicFile::GetExist() const
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         if (true == pPointer->QueryDynamicFile(m_path))
         {
            return true;
         }
      }
   }
   return false;
}

// attempt to load the file from the best provider (providers latter in the provider array are "better")
//  what todo about the gap bettween Exist and Load, where provider may change
//  avoid usage of GetExist? but then how do do Best?
void FoundDynamicFile::AsyncLoadBest(const TLoadCallback& loadCallback)
{
   std::shared_ptr< IFileSystemProvider > pProvider;
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         if (true == pPointer->QueryDynamicFile(m_path))
         {
            pProvider = pPointer;
         }
      }
   }
   auto pFileSystem = m_pFileSystem.lock();
   if ((nullptr != pProvider) && (nullptr != pFileSystem))
   {
      auto path(m_path);
      pFileSystem->AddAsyncTask([=](){
         pProvider->AsyncLoadDynamicFile(loadCallback, path);
      });
   } 
   else
   {
      if (nullptr != loadCallback)
      {
         loadCallback(false, nullptr);
      }
   }
}

// attempt to save the data to every provider
void FoundDynamicFile::AsyncSaveAll(const TFileData& data, const TPassableCallback& callback)
{
   auto pFileSystem = m_pFileSystem.lock();
   if (nullptr == pFileSystem)
   {
      return;
   }
   for (const auto& iter: m_arrayProvider)
   {
      auto pProvider = iter.lock();
      if (nullptr != pProvider)
      {
         auto path(m_path);
         pFileSystem->AddAsyncTask([=](){
            pProvider->AsyncSaveDynamicFile(path, data, callback);
         });
      }
   }

   return;
}

// attempt to delete the file from every provider
void FoundDynamicFile::AsyncDeleteAll(const TPassableCallback& callback)
{
   auto pFileSystem = m_pFileSystem.lock();
   if (nullptr == pFileSystem)
   {
      return;
   }
   for (const auto& iter: m_arrayProvider)
   {
      auto pProvider = iter.lock();
      if (nullptr != pProvider)
      {
         auto path(m_path);
         pFileSystem->AddAsyncTask([=](){
            pProvider->AsyncDeleteDynamicFile(path, callback);
         });
      }
   }

   return;
}
