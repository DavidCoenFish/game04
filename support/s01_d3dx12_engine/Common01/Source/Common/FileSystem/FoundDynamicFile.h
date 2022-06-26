#pragma once

/*
dynamic fils can be read, written to, created and deleted
changes to provider may change the contents of file, and if it exists

if we don't have precaculated hash of the file, don't do on change? 
or if provider changes that was used to provide the file reports a change, do an "on change" for each file it provided?
*/
class IFileSystemProvider;
class IFileSystemVisitorFound;

class FoundDynamicFile
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(const bool bError, const TFileData& pFileData) > TLoadCallback;
   typedef std::function< void(const bool bError) > TPassableCallback;

   static std::shared_ptr< FoundDynamicFile > Factory(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
      );

   FoundDynamicFile(
      const std::filesystem::path& path, 
      const int filter,
      const std::vector< std::weak_ptr< IFileSystemProvider > >& arrayProviders,
      const std::weak_ptr< IFileSystemVisitorFound >& pFileSystem
      );
   ~FoundDynamicFile();

   const int GetFilter() const;
   void OnProviderChange(IFileSystemProvider* const pProvider);

   //when the function was called, the file existed
   const bool GetExist() const;

   // attempt to load the file from the best provider (providers latter in the provider array are "better")
   void AsyncLoadBest(const TLoadCallback& loadCallback);
   // attempt to save the data to every provider
   void AsyncSaveAll(const TFileData& data, const TPassableCallback& saveCallback);
   // attempt to delete the file from every provider
   void AsyncDeleteAll(const TPassableCallback& saveCallback);

   //// trigger callback if the best files has a diffierent hash, callback triggers as soon as run as you don't start with a file, thus it has changed
   //void AddCallbackChangeBest(const TLoadCallback& loadCallback);
   //void RemoveCallbackChangeBest(const TLoadCallback& loadCallback);

private:
   // constant after ctor
   const std::filesystem::path m_path;
   const int m_filter;
   const std::vector< std::weak_ptr< IFileSystemProvider > > m_arrayProvider;
   const std::weak_ptr< IFileSystemVisitorFound > m_pFileSystem;

};

