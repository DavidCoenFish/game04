#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"

class ProviderMods : public IFileSystemProvider
{
public:
   ProviderMods(const int filter);
   ~ProviderMods();

   //get told to do the startup work on some thread when things are ready, once done we tell IFileSystemVisitorProvider we are ready
   // have a dependency on base file system for the list of active mods, or is that external
   //void DoStartupWork();

   void AddMod(const std::filesystem::path& path);
   void RemoveMod(const std::filesystem::path& path);

   virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   virtual void AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) override;
   virtual void RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) override;

   virtual void AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) override;
   virtual void RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) override;
   virtual void AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) override;
   virtual void RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) override;

private:

private:
   int m_filter;

};
