#include "CommonPCH.h"

#include "Common/FileSystem/OverlayFileSystem.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"
#include "Common/FileSystem/IFileSystemVisitorProvider.h"
#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/FoundStaticFolder.h"
#include "Common/FileSystem/FoundDynamicFile.h"
#include "Common/FileSystem/FoundDynamicFolder.h"
#include "Common/Util/Utf8.h"
#include "Common/Worker/WorkerCollection.h"
#include "Common/Macro.h"
#include "Common/Worker/WorkerCount.h"

static const int GetNewFilter()
{
   static int sFilterCount = 0;
   int value = 1 << sFilterCount;
   DSC_ASSERT(sFilterCount < 31);
   sFilterCount += 1;
   return value;
}

class FileSystemInternal : public IFileSystemVisitorFound, public IFileSystemVisitorProvider
{
public:
   typedef std::function< void(void) > TVoidCallback;
   typedef std::function< void(const bool bError, const TFileData& pFileData) > TLoadCallback;
   typedef std::function< void(const bool bError) > TPassableCallback;
   typedef uint32_t TFileHash;

   FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);
   ~FileSystemInternal();
   void SetWeakRefSelf(const std::weak_ptr< IFileSystemVisitorFound >& pFileSystemVisitorFound);

   std::shared_ptr< FoundStaticFile > FindStaticFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundStaticFile > FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter);
   std::shared_ptr< FoundStaticFolder > FindStaticFolder(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFile > FindDynamicFile(const std::filesystem::path& path, const int filter);
   std::shared_ptr< FoundDynamicFolder > FindDynamicFolder(const std::filesystem::path& path, const int filter);
   void AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback);

   void AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter);
   void GatherStaticFolderContents(const std::filesystem::path& path, std::set<std::filesystem::path>& childFiles, std::set<std::filesystem::path>& childFolders, const int filter);
   void AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter);
   void AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& passable, const int filter);
   void AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const TPassableCallback& passable, const int filter);

private:
   void GatherProviders(std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProvider, const int filter);
   IFileSystemProvider* const GetBestProvider(const std::filesystem::path& path, const int filter);

   //IFileSystemVisitorFound
   virtual void AddAsyncTask(const TVoidCallback& callback) override;
   //virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   //virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;

   //IFileSystemVisitorProvider
   virtual void OnReady(const IFileSystemProvider* pProvider) override;
   //virtual void OnStaticFilesChange(IFileSystemProvider* const pProvider) override;

private:
   std::weak_ptr< IFileSystemVisitorFound > m_pFileSystemVisitorFound;
   WorkerCollection<FILE_SYSTEM_WORKER_COUNT> m_workerCollection;

   //no mutex, constant after ctor
   std::vector< std::shared_ptr< IFileSystemProvider > > m_arrayProvider;
   typedef std::weak_ptr< IFileSystemProvider > TWeakProvider;
   std::vector< std::pair< int, TWeakProvider > > m_arrayFilterOverlay;
   std::vector< std::pair< int, IFileSystemProvider* > > m_arrayFilterOverlay2;

   //can change on multiple threads after ctor, under mutex
   std::mutex m_arrayReadyCallbackMutex;
   std::vector< std::pair< int, TVoidCallback > > m_arrayReadyCallback;
   int m_filterReady;

   /*
   do we need to make this one map per provider? to save itterating over so many on provider change?
   changing what files a provider has is always going to be expencive? minimise how many found file we keep active
   allow a path to load files skipping the creation of the found static file object?

   //weak or raw, we are a cache, not a point of ownership.
   std::mutex m_setFoundStaticFileMutex;
   std::set<FoundStaticFile*> m_setFoundStaticFile;
   */

};

FileSystemInternal::FileSystemInternal(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
   : m_arrayProvider(arrayOverlay)
   , m_filterReady(0)
{
   for (const auto& iter : m_arrayProvider)
   {
      const int filter = GetNewFilter();
      iter->SetFilter(filter);
      iter->SetFileSystemVisitorProvider(this);
      m_arrayFilterOverlay.push_back(std::pair< int, TWeakProvider >(filter, iter));
      m_arrayFilterOverlay2.push_back(std::pair< int, IFileSystemProvider* >(filter, iter.get()));
   }
   return;
}

FileSystemInternal::~FileSystemInternal()
{
   //abort any outstanding work. we have had a request to shutdown
   //or don't abort, we where told to do something?
   //m_workerCollection.ClearWork();
}

void FileSystemInternal::SetWeakRefSelf(const std::weak_ptr< IFileSystemVisitorFound >& pFileSystemVisitorFound)
{
   m_pFileSystemVisitorFound = pFileSystemVisitorFound;
}


std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFile(const std::filesystem::path& path, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundStaticFile::Factory( path, filter, arrayProvider, m_pFileSystemVisitorFound );
   return pFoundFile;
}

void FileSystemInternal::GatherProviders(std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProvider, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay)
   {
      if (0 == (iter.first & filter))
      {
         continue;
      }

      arrayProvider.push_back(std::weak_ptr< IFileSystemProvider >(iter.second));
   }
   return;
}

std::shared_ptr< FoundStaticFile > FileSystemInternal::FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundStaticFile::Factory( path, filter, arrayProvider, m_pFileSystemVisitorFound, priorityExtention );
   return pFoundFile;
}

std::shared_ptr< FoundStaticFolder > FileSystemInternal::FindStaticFolder(const std::filesystem::path& path, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFolder = FoundStaticFolder::Factory( path, filter, arrayProvider, m_pFileSystemVisitorFound );
   return pFoundFolder;
}

std::shared_ptr< FoundDynamicFile > FileSystemInternal::FindDynamicFile(const std::filesystem::path& path, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFile = FoundDynamicFile::Factory( path, filter, arrayProvider, m_pFileSystemVisitorFound );
   return pFoundFile;
}

std::shared_ptr< FoundDynamicFolder > FileSystemInternal::FindDynamicFolder(const std::filesystem::path& path, const int filter)
{
   std::vector< std::weak_ptr< IFileSystemProvider > > arrayProvider;
   GatherProviders(arrayProvider, filter);
   if (true == arrayProvider.empty())
   {
      return nullptr;
   }

   auto pFoundFolder = FoundDynamicFolder::Factory( path, filter, arrayProvider, m_pFileSystemVisitorFound );
   return pFoundFolder;
}

void FileSystemInternal::AddCallbackAllProvidersReady(const int filter, const TVoidCallback& callback)
{
   std::lock_guard lock(m_arrayReadyCallbackMutex);
   m_arrayReadyCallback.push_back(std::pair< int, TVoidCallback >(filter, callback));
   return;
}

IFileSystemProvider* const FileSystemInternal::GetBestProvider(const std::filesystem::path& path, const int filter)
{
   for (auto iter = m_arrayFilterOverlay2.rbegin(); 
           iter != m_arrayFilterOverlay2.rend(); ++iter )
   { 
      if (0 == (filter & iter->first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter->second;
      if (nullptr == pProvider)
      {
         continue;
      }
      TFileHash hash = 0;
      if (pProvider->QueryStaticFile(hash, path))
      {
         return pProvider;
      }
   }

   return nullptr;
}

void FileSystemInternal::AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   auto pProvider = GetBestProvider(path, filter);

   if (nullptr != pProvider)
   {
      AddAsyncTask([=](){
         pProvider->AsyncLoadStaticFile(callback, path);
      });
   }
   else
   {
      callback(false, nullptr);
   }
   return;
}

void FileSystemInternal::GatherStaticFolderContents(const std::filesystem::path& path, std::set<std::filesystem::path>& childFiles, std::set<std::filesystem::path>& childFolders, const int filter)
{
   childFiles.clear();
   childFolders.clear();

   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      pProvider->GatherStaticFolderContents(childFiles, childFolders, path);
   }
}

void FileSystemInternal::AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   auto pProvider = GetBestProvider(path, filter);

   if (nullptr != pProvider)
   {
      AddAsyncTask([=](){
         pProvider->AsyncLoadDynamicFile(callback, path);
      });
   }
   else
   {
      callback(false, nullptr);
   }
   return;
}

void FileSystemInternal::AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& passable, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      AddAsyncTask([=](){
         pProvider->AsyncSaveDynamicFile(path, data, passable);
      });
   }
}

void FileSystemInternal::AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const TPassableCallback& passable, const int filter)
{
   for (const auto& iter : m_arrayFilterOverlay2)
   {
      if (0 == (filter & iter.first))
      {
         continue;
      }
      IFileSystemProvider* pProvider = iter.second;
      if (nullptr == pProvider)
      {
         continue;
      }
      AddAsyncTask([=](){
         pProvider->AsyncDeleteDynamicFile(path, passable);
      });
   }
}


//IFileSystemProviderVisitor
void FileSystemInternal::OnReady(const IFileSystemProvider* pProvider)
{
   std::vector< TVoidCallback > pendingReadyCallback;
   {
      std::lock_guard lock(m_arrayReadyCallbackMutex);
      m_filterReady |= pProvider->GetFilter();

      auto copyArrayReadyCallback = m_arrayReadyCallback;
      m_arrayReadyCallback.clear();

      for (const auto& iter : m_arrayReadyCallback)
      {
         if (iter.first == (m_filterReady & iter.first))
         {
            pendingReadyCallback.push_back(iter.second);
         }
         else
         {
            m_arrayReadyCallback.push_back(iter);
         }
      }
   }

   for (const auto& iter : pendingReadyCallback)
   {
      iter();
   }
}

void FileSystemInternal::AddAsyncTask(const TVoidCallback& callback)
{
   m_workerCollection.AddWork(callback);
}

std::shared_ptr< OverlayFileSystem > OverlayFileSystem::Factory(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
{
   return std::make_shared< OverlayFileSystem >(arrayOverlay);
}

const int OverlayFileSystem::GetFilterAll()
{
   return 0xFFFFFFFF;
}

OverlayFileSystem::OverlayFileSystem(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay)
{
   m_pInternal = std::make_shared<FileSystemInternal>(arrayOverlay);
   //m_pInternalVisitorFound = std::make_shared<IFileSystemVisitorFound>(m_pInternal.get()); 
   std::weak_ptr<IFileSystemVisitorFound> pWeak(m_pInternal);
   m_pInternal->SetWeakRefSelf(pWeak);
}

OverlayFileSystem::~OverlayFileSystem()
{
   m_pInternal = nullptr;
}

std::shared_ptr< FoundStaticFile > OverlayFileSystem::FindStaticFile(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindStaticFile(path, filter);
}

std::shared_ptr< FoundStaticFile > OverlayFileSystem::FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter)
{
   return m_pInternal->FindStaticFilePriorityExtention(path, priorityExtention, filter);
}

std::shared_ptr< FoundStaticFolder > OverlayFileSystem::FindStaticFolder(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindStaticFolder(path, filter);
}

std::shared_ptr< FoundDynamicFile > OverlayFileSystem::FindDynamicFile(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindDynamicFile(path, filter);
}

std::shared_ptr< FoundDynamicFolder > OverlayFileSystem::FindDynamicFolder(const std::filesystem::path& path, const int filter)
{
   return m_pInternal->FindDynamicFolder(path, filter);
}

void OverlayFileSystem::AddCallbackAllProvidersReady(const TVoidCallback& callback, const int filter)
{
   return m_pInternal->AddCallbackAllProvidersReady(filter, callback);
}

void OverlayFileSystem::AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   return m_pInternal->AsyncStaticFileLoadBest(path, callback, filter);
}

void OverlayFileSystem::GatherStaticFolderContents(const std::filesystem::path& path, std::set<std::filesystem::path>& childFiles, std::set<std::filesystem::path>& childFolders, const int filter)
{
   return m_pInternal->GatherStaticFolderContents(path, childFiles, childFolders, filter);
}

void OverlayFileSystem::AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter)
{
   return m_pInternal->AsyncDynamicFileLoadBest(path, callback, filter);
}

void OverlayFileSystem::AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& callback, const int filter)
{
   return m_pInternal->AsyncDynamicFileSaveAll(path, data, callback, filter);
}
void OverlayFileSystem::AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const TPassableCallback& callback, const int filter)
{
   return m_pInternal->AsyncDynamicFileDeleteAll(path, callback, filter);
}


