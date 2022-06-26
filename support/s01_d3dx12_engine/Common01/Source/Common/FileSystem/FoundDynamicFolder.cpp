#include "CommonPCH.h"

#include "Common/FileSystem/FoundDynamicFolder.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

std::shared_ptr< FoundDynamicFolder > FoundDynamicFolder::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
{
   return std::make_shared<FoundDynamicFolder>(
      path, 
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundDynamicFolder::FoundDynamicFolder(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
   : m_path(path)
   , m_filter(filter)
   , m_arrayProvider(arrayProviders)
   , m_pFileSystem(pFileSystem)
   , m_found(false)
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->AddFoundDynamicFolder(this);
      }
   }
}

FoundDynamicFolder::~FoundDynamicFolder()
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->RemoveFoundDynamicFolder(this);
      }
   }
}

const int FoundDynamicFolder::GetFilter() const
{
   return m_filter;
}

void FoundDynamicFolder::OnProviderChange(IFileSystemProvider* const)
{
   bool found = false;
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         found |= pPointer->QueryDynamicFolder(m_path);
      }
   }

   {
      std::lock_guard lock(m_foundMutex);
      m_found = found;
   }

   //callbacks?
}

const bool FoundDynamicFolder::GetExist() const
{
   std::lock_guard lock(m_foundMutex);
   return m_found;
}

void FoundDynamicFolder::GatherDynamicFolderContents(
   std::set<std::filesystem::path>& childFiles, 
   std::set<std::filesystem::path>& childFolders
   )
{
   childFiles.clear();
   childFolders.clear();

   for (const auto& iter: m_arrayProvider)
   {
      auto pProvider = iter.lock();
      if (nullptr != pProvider)
      {
         pProvider->GatherDynamicFolderContents(childFiles, childFolders, m_path);
      }
   }

   return;
}

void FoundDynamicFolder::AsyncCreateFolderAll(const TPassableCallback& callback)
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
            pProvider->AsyncCreateDynamicFolder(path, callback);
         });
      }
   }
}

void FoundDynamicFolder::AsyncRemoveFolderAll(const TPassableCallback& callback)
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
            pProvider->AsyncDeleteDynamicFolder(path, callback);
         });
      }
   }
}

