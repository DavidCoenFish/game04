#pragma once

class IFileSystemProvider;
class IFileSystemVisitorFound;
class FileSystemInternal;
class FoundStaticFile;
class FoundStaticFolder;
class FoundDynamicFile;
class FoundDynamicFolder;

/*
===========issues
what if log want consumer that uses file system, it can save to memory till file system is ready, but how does it know when the file system is ready
   or is "FileSystem" too heavy/ mod orientated for log? collect basic file system methods into FileSystemUtil 

rename FileSystem => OverlayFileSystem, FileSystemUtil=>FileSystem

===========concept
dynamic and static do not exist in the same mapping of the filesystem, both use relative paths.
"best" is in reference to the higher the index of the provider, the higher the priority which makes it "Better". filter mask = 1 << index
"all" is to allow action on every provider that passes the filter mask
"static" meaning they can not be edited, a provider may change what is provided is however
 static "/test.txt" as <foo> from provider A, when provider B is ready, it may provide static "/test.txt" as <foo2>
"dynamic" can be changed via the file system, save, delete, create, as well as load

may not need to change avaliablity/ order of overlays at runtime, thought that may result in mods having to do more work, 
   ie. one provider that internally manages mod order and being on/off for each mod
   this could look like having the file system created early, with placeholders provider, which lets observers know when things change/ are ready?

do the providers use the file system AddAsyncTask to do startup work, or it that the application's job? application's job.

we can find files/ folders that don't exist (yet)? rename Found -> Handel?

does "on ready" also trigger "on change" on any provided files? it probably should

===========interfaces
IFileSystemProvider
   SetFilter() //for FileSystem only?
   GetFilter()
   SetCallbacks(ready, change) //onReady/onChange return a map of all the files/ locations, hash of static files...
   AsyncStaticFileLoad()
   AsyncDynamicFileLoad()
   AsyncDynamicFileSave()
   AsyncDynamicFileDelete()
   AsyncDynamicFolderMake() //so, does this update map of all files, it probably should, are we going to end up with static and dynamic maps of files?
   AsyncDynamicFolderRemove()

FoundStaticFile
   GetExists
   AsyncLoadBest,
   AddCallbackChangeBest(path, filter, callback), //first thing it does is return the current file data? redundant with AsyncLoadBest but keeps data in step with changes
   RemoveCallback(path, filter, callback)
   //AsyncLoadAll, is not needed?

FoundStaticFolder //must exisit to be returned
   GetExists
   GatherStaticFolderContents //visits each provider passing filter
   //AddCallbackChange, implement if needed, requires keeping a hash of each "best" file?

FoundDynamicFile 
   GetExists
   AsyncLoadBest,
   AsyncSaveAll,
   AsyncDeleteAll,
   //AddCallbackChange, implement if needed, trigger if any provider in use changes?
   //?AddCallbackChangeLoadBest

FoundDynamicFolder
   GetExists
   AsyncCreate,
   AsyncRemove,
   GatherDynamicFolderContents
   //AddCallbackChange, implement if needed, triiger if any provider in use changes?

*/

class OverlayFileSystem
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(void) > TVoidCallback;
   // re error. do we need a message, info on which provider? or is a black box "it did not work" sufficient
   typedef std::function< void(const bool bError, const TFileData& pFileData) > TLoadCallback;
   typedef std::function< void(const bool bError) > TPassableCallback;
   typedef uint32_t TFileHash;

   // we construct the file system with an array of providers (latter in the array is higher priority), however they need to tell the file system when they are ready
   static std::shared_ptr< OverlayFileSystem > Factory(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);

   static const int GetFilterAll();

   OverlayFileSystem(const std::vector<std::shared_ptr< IFileSystemProvider > >& arrayOverlay);
   ~OverlayFileSystem();

   // the found objects are to get full interface for actions like "add on change callback"
   std::shared_ptr< FoundStaticFile > FindStaticFile(const std::filesystem::path& path, const int filter = GetFilterAll());
   std::shared_ptr< FoundStaticFile > FindStaticFilePriorityExtention(const std::filesystem::path& path, const std::vector<std::string>& priorityExtention, const int filter = GetFilterAll());
   std::shared_ptr< FoundStaticFolder > FindStaticFolder(const std::filesystem::path& path, const int filter = GetFilterAll());
   std::shared_ptr< FoundDynamicFile > FindDynamicFile(const std::filesystem::path& path, const int filter = GetFilterAll());
   std::shared_ptr< FoundDynamicFolder > FindDynamicFolder(const std::filesystem::path& path, const int filter = GetFilterAll());

   //static? what if log wants a consumer that uses file system
   // only callsback on "all" the providers of filter being ready?
   // filter bit [2, 4], then providers [2,4] must be ready before callback is called
   void AddCallbackAllProvidersReady(const TVoidCallback& callback, const int filter = GetFilterAll());

   //helper functions to save people from having to do a lot of work for basic functions
   // 
   // WARNING, is this safe to use if file will just get replace by mod-ed version latter? we might not care for the range of application scope required.
   // load the file of the best matching provider
   void AsyncStaticFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter = GetFilterAll());
   void GatherStaticFolderContents(const std::filesystem::path& path, std::set<std::filesystem::path>& childFiles, std::set<std::filesystem::path>& childFolders, const int filter = GetFilterAll());
   void AsyncDynamicFileLoadBest(const std::filesystem::path& path, const TLoadCallback& callback, const int filter = GetFilterAll());

   // save to all provider that pass filter and allow dynamic save
   void AsyncDynamicFileSaveAll(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& callback = nullptr, const int filter = GetFilterAll());
   void AsyncDynamicFileDeleteAll(const std::filesystem::path& path, const TPassableCallback& callback = nullptr, const int filter = GetFilterAll());

   void GatherDynamicFolderContents(const std::filesystem::path& path, std::set<std::filesystem::path>& childFiles, std::set<std::filesystem::path>& childFolders, const int filter = GetFilterAll());
   //AsyncDynamicFolderCreateAll?
   //AsyncDynamicFolderDeleteAll?

private:
   std::shared_ptr< FileSystemInternal > m_pInternal;
   //std::shared_ptr< IFileSystemVisitorFound > m_pInternalVisitorFound;

};
