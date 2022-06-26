#include "CommonPCH.h"

#include "Common/FileSystem/FoundStaticFile.h"
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/IFileSystemVisitorFound.h"

static void LoadStaticFile(
   const std::shared_ptr< IFileSystemVisitorFound >& pFileSystem, 
   const FoundStaticFile::TLoadCallback& loadCallback,
   const bool found,
   const std::filesystem::path& path,
   IFileSystemProvider* pBestProvider
   )
{
   if (false == found)
   {
      if (nullptr != loadCallback)
      {
         loadCallback(false, nullptr);
      }
      return;
   }
   if ((nullptr != pBestProvider) && (nullptr != pFileSystem))
   {
      pFileSystem->AddAsyncTask([=](){
         pBestProvider->AsyncLoadStaticFile(loadCallback, path);
      });
   }
}

std::shared_ptr< FoundStaticFile > FoundStaticFile::Factory(
   const std::filesystem::path& path, 
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem,
   const std::vector<std::string>& priorityExtention
   )
{
   std::vector< std::filesystem::path > pathArray;
   if (0 == priorityExtention.size())
   {
      pathArray.push_back(path);
   }
   else
   {
      for (const auto& iter : priorityExtention)
      {
         auto copyPath(path);
         copyPath.replace_extension(iter);
         pathArray.push_back(copyPath);
      }
   }
   return std::make_shared<FoundStaticFile>(
      pathArray,
      filter,
      arrayProviders,
      pFileSystem
      );
}

FoundStaticFile::FoundStaticFile(
   const std::vector<std::filesystem::path>& pathArray,
   const int filter,
   const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
   const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
   )
   : m_pathArray(pathArray)
   , m_filter(filter)
   , m_arrayProvider(arrayProviders)
   , m_pFileSystem(pFileSystem)
   , m_pBestProvider()
   , m_bestHash(0)
   , m_bestFound(false)
{
   //m_pFileSystem->AddFoundStaticFile(this);
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->AddFoundStaticFile(this);
      }
   }
   EvalueBest();
   return;
}

FoundStaticFile::~FoundStaticFile()
{
   //m_pFileSystem->RemoveFoundStaticFile(this);
   for (const auto& iter: m_arrayProvider)
   {
      auto pPointer = iter.lock();
      if (nullptr != pPointer)
      {
         pPointer->RemoveFoundStaticFile(this);
      }
   }

   return;
}

const int FoundStaticFile::GetFilter() const
{
   return m_filter;
}

void FoundStaticFile::OnProviderChange(IFileSystemProvider* const)
{
   if (false == EvalueBest())
   {
      return;
   }

   std::shared_ptr< IFileSystemProvider> pBestProvider = nullptr;
   std::vector< TLoadCallback > arrayCallbackBestChange;
   std::filesystem::path bestPath;
   bool found = false;
   {
      std::lock_guard lock(m_bestMutex);
      arrayCallbackBestChange = m_arrayCallbackBestChange;
      pBestProvider = m_pBestProvider.lock();
      bestPath = m_bestPath;
      found = m_bestFound;
   }
   if (nullptr != pBestProvider)
   {
      for (const auto& iter : arrayCallbackBestChange)
      {
         LoadStaticFile(m_pFileSystem.lock(), iter, found, bestPath, pBestProvider.get());
      }
   }
}

//when the function was called, the file existed
const bool FoundStaticFile::GetExist() const
{
   std::lock_guard lock(m_bestMutex);
   return m_bestFound;
}

//load the best file that passes filter, return false if file doesn't exist
void FoundStaticFile::AsyncLoadBest(const TLoadCallback& loadCallback)
{
   std::shared_ptr< IFileSystemProvider> pBestProvider = nullptr;
   std::filesystem::path bestPath;
   bool found = false;
   {
      std::lock_guard lock(m_bestMutex);
      pBestProvider = m_pBestProvider.lock();
      bestPath = m_bestPath;
      found = m_bestFound;
   }
   LoadStaticFile(m_pFileSystem.lock(), loadCallback, found, bestPath, pBestProvider.get());
}

//load each file that passes filter (ie, from possibly multiple providers)
//void AsyncLoadAll(const TLoadCallback& loadCallback);
   
// trigger callback if the best files has a diffierent hash, callback triggers as soon as run as you don't start with a file, thus it has changed?
void FoundStaticFile::AddCallbackChangeBest(const TLoadCallback& loadCallback)
{
   std::shared_ptr< IFileSystemProvider> pBestProvider = nullptr;
   std::filesystem::path bestPath;
   bool found = false;
   {
      std::lock_guard lock(m_bestMutex);
      m_arrayCallbackBestChange.push_back(loadCallback);
      pBestProvider = m_pBestProvider.lock();
      bestPath = m_bestPath;
      found = m_bestFound;
   }
   LoadStaticFile(m_pFileSystem.lock(), loadCallback, found, bestPath, pBestProvider.get());
   return;
}

//https://stackoverflow.com/questions/20833453/comparing-stdfunctions-for-equality
template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

void FoundStaticFile::RemoveCallbackChangeBest(const TLoadCallback& loadCallback)
{
   std::lock_guard lock(m_bestMutex);
   const size_t lhs = getAddress(loadCallback);
   m_arrayCallbackBestChange.erase(
      std::remove_if(m_arrayCallbackBestChange.begin(), m_arrayCallbackBestChange.end(), [=](const TLoadCallback& rhs){
         return (lhs == getAddress(rhs));
      }), 
      m_arrayCallbackBestChange.end()
      );

   return;
}

const bool FoundStaticFile::EvalueBest()
{
   const TFileHash oldBestHash(m_bestFound);
   m_bestFound = false;
   m_bestHash = 0;
   m_bestPath = std::filesystem::path();
   m_pBestProvider.reset();

   std::lock_guard lock(m_bestMutex);
   for (const auto& iter: m_arrayProvider)
   {
      auto pProvider = iter.lock();
      if (nullptr == pProvider)
      {
         continue;
      }
      for (const auto& path: m_pathArray)
      {
         TFileHash hash = 0;
         if (pProvider->QueryStaticFile(hash, path))
         {
            m_bestFound = true;
            m_bestHash = hash;
            m_pBestProvider = iter;
            m_bestPath = path;
            break;
         }
      }
      if (true == m_bestFound)
      {
         break;
      }
   }

   //want callback change to trigger when file is removed?
   //return ((true == m_bestFound) && (oldBestHash != m_bestHash));
   return (oldBestHash != m_bestHash);
}
