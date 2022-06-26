#include "CommonPCH.h"

#include "Common/FileSystem/FoundStaticFolder.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

std::shared_ptr< FoundStaticFolder > FoundStaticFolder::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
{
   return std::make_shared<FoundStaticFolder>(
      path, 
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundStaticFolder::FoundStaticFolder(
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
         pPointer->AddFoundStaticFolder(this);
         m_found |= pPointer->QueryStaticFolder(m_path);
      }
   }

   return;
}

FoundStaticFolder::~FoundStaticFolder()
{
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->RemoveFoundStaticFolder(this);
      }
   }

   return;
}

const int FoundStaticFolder::GetFilter() const
{
   return m_filter;
}

void FoundStaticFolder::OnProviderChange(IFileSystemProvider* const)
{
   bool found = false;
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         found |= pPointer->QueryStaticFolder(m_path);
      }
   }

   {
      std::lock_guard lock(m_foundMutex);
      m_found = found;
   }

   //callbacks?
}

//when the function was called, the file existed
const bool FoundStaticFolder::GetExist() const
{
   std::lock_guard lock(m_foundMutex);
   return m_found;
}


void FoundStaticFolder::GatherStaticFolderContents(
   const std::filesystem::path& path, 
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
         pProvider->GatherStaticFolderContents(childFiles, childFolders, path);
      }
   }

   return;
}

