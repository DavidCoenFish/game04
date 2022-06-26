#pragma once
#include "Common/FileSystem/IFileSystemProvider.h"
#include "Common/FileSystem/ComponentFileMap.h"

class FoundStaticFile;
class FoundStaticFolder;
class FoundDynamicFile;
class FoundDynamicFolder;

/*
this was intened for 
*/

//struct TComponentFileMapEmpty;
//template <typename TStaticFileData = TComponentFileMapEmpty, typename TDynamicFileData = TComponentFileMapEmpty>
//class ComponentFileMap;

class ComponentFound;

class ProviderMemory : public IFileSystemProvider
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef uint32_t TFileHash;
   struct TStaticFileData
   {
      TFileHash m_fileHash;
      TFileData m_fileData;
   };
   typedef std::shared_ptr< TStaticFileData > TPointerStaticFileData;
   struct TDynamicFileData
   {
      TFileData m_fileData;
   };
   typedef TFileData TPointerDynamicFileData;
   typedef ComponentFileMap< TPointerStaticFileData > TComponentStaticFileMap;
   typedef std::shared_ptr< TComponentStaticFileMap > TPointerComponentStaticFileMap;
   typedef ComponentFileMap< TPointerDynamicFileData > TComponentDynamicFileMap;
   typedef std::shared_ptr< TComponentDynamicFileMap > TPointerComponentDynamicFileMap;

   typedef std::map< std::filesystem::path, TPointerStaticFileData > TStaticFileMap;
   typedef std::map< std::filesystem::path, TPointerDynamicFileData > TDynamicFileMap;

   static std::shared_ptr<ProviderMemory> Factory(
      const TStaticFileMap& staticFiles = TStaticFileMap(),
      const TDynamicFileMap& dynamicFiles = TDynamicFileMap()
      );
   ProviderMemory(
      const TPointerComponentStaticFileMap& pComponentStaticFileMap,
      const TPointerComponentDynamicFileMap& pComponentDynamicFileMap
      );
   virtual ~ProviderMemory();

private:
   virtual void SetFilter(const int filter) override;
   virtual const int GetFilter()const override;
   virtual void SetFileSystemVisitorProvider(IFileSystemVisitorProvider* const pVisitor) override;
   virtual const bool QueryStaticFile(TFileHash& hashIfFound, const std::filesystem::path& path) override;
   virtual void AsyncLoadStaticFile(const TLoadCallback& loadCallback, const std::filesystem::path& path) override;

   virtual const bool QueryStaticFolder(const std::filesystem::path& path) override;
   virtual const bool GatherStaticFolderContents(
      std::set< std::filesystem::path >& childFiles,
      std::set< std::filesystem::path >& childFolders,
      const std::filesystem::path& path
      ) override;

   virtual const bool QueryDynamicFile(const std::filesystem::path& path) override;
   virtual void AsyncLoadDynamicFile(const TLoadCallback& loadCallback, const std::filesystem::path& path) override;
   virtual void AsyncSaveDynamicFile(const std::filesystem::path& path, const TFileData& data, const TPassableCallback& callback) override;
   virtual void AsyncDeleteDynamicFile(const std::filesystem::path& path, const TPassableCallback& callback) override;

   virtual const bool QueryDynamicFolder(const std::filesystem::path& path) override;
   virtual const bool GatherDynamicFolderContents(
      std::set< std::filesystem::path >& childFiles,
      std::set< std::filesystem::path >& childFolders,
      const std::filesystem::path& path
      ) override;
   virtual void AsyncCreateDynamicFolder(const std::filesystem::path& path, const TPassableCallback& callback) override;
   virtual void AsyncDeleteDynamicFolder(const std::filesystem::path& path, const TPassableCallback& callback) override;

   virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) override;
   virtual void AddFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) override;
   virtual void RemoveFoundStaticFolder(FoundStaticFolder* const pFoundStaticFolder) override;

   virtual void AddFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) override;
   virtual void RemoveFoundDynamicFile(FoundDynamicFile* const pFoundDynamicFile) override;
   virtual void AddFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) override;
   virtual void RemoveFoundDynamicFolder(FoundDynamicFolder* const pFoundDynamicFolder) override;


private:
   int m_filter;
   IFileSystemVisitorProvider* m_pVisitor;
   TPointerComponentStaticFileMap m_pComponentStaticFileMap;
   TPointerComponentDynamicFileMap m_pComponentDynamicFileMap;
   std::unique_ptr< ComponentFound > m_pComponentFound;

};
