#pragma once

class FoundStaticFile;
class FoundStaticFolder;
class FoundDynamicFile;
class FoundDynamicFolder;
class IFileSystemProvider;

//weak or raw, we are a cache, not a point of ownership.
class ComponentFound
{
public:
   ComponentFound();
   ~ComponentFound();

   void OnProviderChangeStatic(IFileSystemProvider* const pProvider);
   void OnProviderChangeDynamic(IFileSystemProvider* const pProvider);

   void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile);
   void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile);
   void AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder);
   void RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder);
   void AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile);
   void RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile);
   void AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder);
   void RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder);

private:
   std::mutex m_setFoundStaticFileMutex;
   std::set<FoundStaticFile*> m_setFoundStaticFile;

   std::mutex m_setFoundStaticFolderMutex;
   std::set<FoundStaticFolder*> m_setFoundStaticFolder;

   std::mutex m_setFoundDynamicFileMutex;
   std::set<FoundDynamicFile*> m_setFoundDynamicFile;

   std::mutex m_setFoundDynamicFolderMutex;
   std::set<FoundDynamicFolder*> m_setFoundDynamicFolder;

};
