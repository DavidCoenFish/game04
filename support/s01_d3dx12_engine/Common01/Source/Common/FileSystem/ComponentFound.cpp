#include "CommonPCH.h"

#include "Common/FileSystem/ComponentFound.h"
#include "Common/FileSystem/FoundDynamicFile.h"
#include "Common/FileSystem/FoundDynamicFolder.h"
#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/FoundStaticFolder.h"

ComponentFound::ComponentFound()
{
}
ComponentFound::~ComponentFound()
{
}

void ComponentFound::OnProviderChangeStatic(IFileSystemProvider* const pProvider)
{
   {
      std::lock_guard lock(m_setFoundStaticFileMutex);
      for (const auto& iter: m_setFoundStaticFile)
      {
         iter->OnProviderChange(pProvider);
      }
   }
   {
      std::lock_guard lock(m_setFoundStaticFolderMutex);
      for (const auto& iter: m_setFoundStaticFolder)
      {
         iter->OnProviderChange(pProvider);
      }
   }
}

void ComponentFound::OnProviderChangeDynamic(IFileSystemProvider* const pProvider)
{
   {
      std::lock_guard lock(m_setFoundDynamicFileMutex);
      for (const auto& iter: m_setFoundDynamicFile)
      {
         iter->OnProviderChange(pProvider);
      }
   }
   {
      std::lock_guard lock(m_setFoundDynamicFolderMutex);
      for (const auto& iter: m_setFoundDynamicFolder)
      {
         iter->OnProviderChange(pProvider);
      }
   }
}

void ComponentFound::AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   std::lock_guard lock(m_setFoundStaticFileMutex);
   m_setFoundStaticFile.insert(pFoundStaticFile);
}

void ComponentFound::RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile)
{
   std::lock_guard lock(m_setFoundStaticFileMutex);
   m_setFoundStaticFile.erase(pFoundStaticFile);
}

void ComponentFound::AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder)
{
   std::lock_guard lock(m_setFoundStaticFolderMutex);
   m_setFoundStaticFolder.insert(pFoundStaticFolder);
}

void ComponentFound::RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder)
{
   std::lock_guard lock(m_setFoundStaticFolderMutex);
   m_setFoundStaticFolder.erase(pFoundStaticFolder);
}

void ComponentFound::AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile)
{
   std::lock_guard lock(m_setFoundDynamicFileMutex);
   m_setFoundDynamicFile.insert(pFoundDynamicFile);
}

void ComponentFound::RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile)
{
   std::lock_guard lock(m_setFoundDynamicFileMutex);
   m_setFoundDynamicFile.erase(pFoundDynamicFile);
}

void ComponentFound::AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder)
{
   std::lock_guard lock(m_setFoundDynamicFileMutex);
   m_setFoundDynamicFolder.insert(pFoundDynamicFolder);
}

void ComponentFound::RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder)
{
   std::lock_guard lock(m_setFoundDynamicFileMutex);
   m_setFoundDynamicFolder.erase(pFoundDynamicFolder);
}

