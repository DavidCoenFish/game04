#include "CommonPCH.h"

#include "Common/FileSystem/ProviderMemory.h"
#include "Common/FileSystem/ComponentFileMap.h"
#include "Common/FileSystem/ComponentFound.h"
#include "Common/FileSystem/IFileSystemVisitorProvider.h"

std::shared_ptr<ProviderMemory> ProviderMemory::Factory(
   const TStaticFileMap& staticFiles,
   const TDynamicFileMap& dynamicFiles
   )
{
   auto pComponentStaticFileMap = TComponentStaticFileMap::Factory(staticFiles);
   auto pComponentDynamicFileMap = TComponentDynamicFileMap::Factory(dynamicFiles);
   return std::make_shared<ProviderMemory>( pComponentStaticFileMap, pComponentDynamicFileMap );
}

ProviderMemory::ProviderMemory(
   const TPointerComponentStaticFileMap& pComponentStaticFileMap,
   const TPointerComponentDynamicFileMap& pComponentDynamicFileMap
   )
   : m_filter(0)
   , m_pVisitor(nullptr)
   , m_pComponentStaticFileMap(pComponentStaticFileMap)
   , m_pComponentDynamicFileMap(pComponentDynamicFileMap)
{
   m_pComponentFound = std::make_unique<ComponentFound>();
   return;
}

ProviderMemory::~ProviderMemory()
{
   m_pComponentFound = nullptr;
   return;
}

void ProviderMemory::SetFilter(const int filter)
{
   m_filter = filter;
}

const int ProviderMemory::GetFilter()const
{
   return m_filter;
}

void ProviderMemory::SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor)
{
   m_pVisitor = pVisitor;
   {
      //we are about to set ourself as ready, do we need to tell any found files/folders already using us that we are going to change?
      //m_pComponentFound->OnProviderChangeStatic(this);
      //m_pComponentFound->OnProviderChangeDynamic(this);
   }
   m_pVisitor->OnReady(this);
}

const bool ProviderMemory::QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path)
{
   TPointerStaticFileData pData;
   if (true == m_pComponentStaticFileMap->GetFile(path, pData))
   {
      hashIfFound = pData->m_fileHash;
      return true;
   }
   hashIfFound = 0;
   return false;
}

void ProviderMemory::AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path)
{
   TPointerStaticFileData pData;
   if (true == m_pComponentStaticFileMap->GetFile(path, pData))
   {
      loadCallback(false, pData->m_fileData);
   }
   else
   {
      loadCallback(true, nullptr);
   }
   return;
}

const bool ProviderMemory::QueryStaticFolder(const std::filesystem::path& path)
{
   return m_pComponentStaticFileMap->HasFolder(path);
}

const bool ProviderMemory::GatherStaticFolderContents(
   std::set< std::filesystem::path >& childFiles,
   std::set< std::filesystem::path >& childFolders,
   const std::filesystem::path& path
   )
{
   return m_pComponentStaticFileMap->GatherFolder(
      path,
      childFiles,
      childFolders
      );
}

const bool ProviderMemory::QueryDynamicFile(const std::filesystem::path& path)
{
   TPointerDynamicFileData pData;
   if (true == m_pComponentDynamicFileMap->GetFile(path, pData))
   {
      return true;
   }
   return false;
}

void ProviderMemory::AsyncLoadDynamicFile(const TLoadCallback& loadCallback, const std::filesystem::path& path)
{
   TPointerDynamicFileData pData;
   if (true == m_pComponentDynamicFileMap->GetFile(path, pData))
   {
      loadCallback(true, pData);
   }
   else
   {
      loadCallback(false, nullptr);
   }
   return;
}

void ProviderMemory::AsyncSaveDynamicFile(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& callback)
{
   m_pComponentDynamicFileMap->AddFile(path, data);
   m_pComponentFound->OnProviderChangeDynamic(this);
   if (nullptr != callback)
   {
      callback(true);
   }
}

void ProviderMemory::AsyncDeleteDynamicFile(const std::filesystem::path& path, const TPassableCallback& callback)
{
   const bool result = m_pComponentDynamicFileMap->RemoveFile(path);
   m_pComponentFound->OnProviderChangeDynamic(this);
   if (nullptr != callback)
   {
      callback(result);
   }
}

const bool ProviderMemory::QueryDynamicFolder(const std::filesystem::path& path)
{
   return m_pComponentDynamicFileMap->HasFolder(path);
}

const bool ProviderMemory::GatherDynamicFolderContents(
   std::set< std::filesystem::path >& childFiles,
   std::set< std::filesystem::path >& childFolders,
   const std::filesystem::path& path
   )
{
   return m_pComponentDynamicFileMap->GatherFolder(
      path,
      childFiles,
      childFolders
      );
}

void ProviderMemory::AsyncCreateDynamicFolder(const std::filesystem::path& path, const TPassableCallback& callback)
{
   const bool result = m_pComponentDynamicFileMap->AddFolder(path);
   m_pComponentFound->OnProviderChangeDynamic(this);
   if (nullptr != callback)
   {
      callback(result);
   }
}

void ProviderMemory::AsyncDeleteDynamicFolder(const std::filesystem::path& path, const TPassableCallback& callback)
{
   const bool result = m_pComponentDynamicFileMap->RemoveFolder(path);
   m_pComponentFound->OnProviderChangeDynamic(this);
   if (nullptr != callback)
   {
      callback(result);
   }
}

void ProviderMemory::AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   m_pComponentFound->AddFoundStaticFile(pFoundStaticFile);
}

void ProviderMemory::RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) 
{
   m_pComponentFound->RemoveFoundStaticFile(pFoundStaticFile);
}

void ProviderMemory::AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) 
{
   m_pComponentFound->AddFoundStaticFolder(pFoundStaticFolder);
}

void ProviderMemory::RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) 
{
   m_pComponentFound->RemoveFoundStaticFolder(pFoundStaticFolder);
}

void ProviderMemory::AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) 
{
   m_pComponentFound->AddFoundDynamicFile(pFoundDynamicFile);
}

void ProviderMemory::RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) 
{
   m_pComponentFound->RemoveFoundDynamicFile(pFoundDynamicFile);
}

void ProviderMemory::AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) 
{
   m_pComponentFound->AddFoundDynamicFolder(pFoundDynamicFolder);
}

void ProviderMemory::RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) 
{
   m_pComponentFound->RemoveFoundDynamicFolder(pFoundDynamicFolder);
}


