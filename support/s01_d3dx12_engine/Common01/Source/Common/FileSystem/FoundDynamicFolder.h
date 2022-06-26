#pragma once

/*
dynamic folders can be created and detroyed
*/
class IFileSystemProvider;
class IFileSystemVisitorFound;

class FoundDynamicFolder
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(const TFileData&) > TLoadCallback;
   typedef std::function< void(const bool bError) > TPassableCallback;
   typedef uint32_t TFileHash;

   static std::shared_ptr< FoundDynamicFolder > Factory(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
      );

   FoundDynamicFolder(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
      );
   ~FoundDynamicFolder();

   const int GetFilter() const;
   void OnProviderChange(IFileSystemProvider* const pProvider);

   const bool GetExist() const;

   void GatherDynamicFolderContents(
      std::set<std::filesystem::path>& childFiles, 
      std::set<std::filesystem::path>& childFolders
      );
   
   void AsyncCreateFolderAll(const TPassableCallback& callback = nullptr);
   void AsyncRemoveFolderAll(const TPassableCallback& callback = nullptr);

private:
   //constant after ctor
   const std::filesystem::path m_path;
   const int m_filter;
   const std::vector< std::weak_ptr< IFileSystemProvider > > m_arrayProvider;
   const std::weak_ptr< IFileSystemVisitorFound > m_pFileSystem;

   //can change any thread
   mutable std::mutex m_foundMutex;
   bool m_found;
};

