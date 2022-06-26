#pragma once

/*
we can not "create" or "delete" static files
changes in providers can however change contents of a static file, and if it exisits
ie, a mod turning on or off, the contents of a file may change, and/or static files may be added or removed

if you want to watch for file change callbacks, just call AddCallbackChangeBest and it will imediatle trigger the callback with the initial best file data
*/

class IFileSystemVisitorFound;
class IFileSystemProvider;

class FoundStaticFile
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(const bool bError, const TFileData& pFileData) > TLoadCallback;
   typedef uint32_t TFileHash;

   static std::shared_ptr< FoundStaticFile > Factory(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem,
      const std::vector<std::string>& priorityExtention = std::vector<std::string>()
      );

   FoundStaticFile(
      const std::vector<std::filesystem::path>& pathArray,
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
      );
   ~FoundStaticFile();

   const int GetFilter() const;
   void OnProviderChange(IFileSystemProvider* const pProvider);

   //when the function was called, the file existed
   const bool GetExist() const;

   //load the best file that passes filter, return error if file doesn't exist
   void AsyncLoadBest(const TLoadCallback& loadCallback);
   //load each file that passes filter (ie, from multiple providers)
   //void AsyncLoadAll(const TLoadCallback& loadCallback);
   
   // trigger callback if the best files has a diffierent hash, callback triggers as soon as run as you don't start with a file, thus it has changed
   void AddCallbackChangeBest(const TLoadCallback& loadCallback);
   void RemoveCallbackChangeBest(const TLoadCallback& loadCallback);

private:
   const bool EvalueBest();

private:
   //constant after ctor
   const std::vector<std::filesystem::path> m_pathArray;
   const int m_filter;
   // weak pointers rather than raw ptr, FileSystem has ownership over provider's lifespan, we don't.
   //std::vector<IFileSystemProvider*> m_arrayProvider;
   const std::vector< std::weak_ptr< IFileSystemProvider > > m_arrayProvider;
   //IFileSystemVisitorFound* m_pFileSystem;
   const std::weak_ptr< IFileSystemVisitorFound > m_pFileSystem;

   //can change any thread
   mutable std::mutex m_bestMutex;
   bool m_bestFound; //ie, file exists
   TFileHash m_bestHash;
   std::weak_ptr< IFileSystemProvider > m_pBestProvider;
   std::filesystem::path m_bestPath;

   //can change any thread
   mutable std::mutex m_arrayCallbackBestChangeMutex;
   std::vector< TLoadCallback > m_arrayCallbackBestChange;

};
