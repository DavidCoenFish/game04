#pragma once
/*
provide some bare bone access to the file system.

see OverlayFileSystem for the intergration of overlays and async functions

*/
namespace FileSystem
{
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;

   const std::filesystem::path GetModualDir(HINSTANCE hInstance);
   const std::filesystem::path GetTempDir();
   const TFileData SyncReadFile(const std::filesystem::path& absolutePath);
   const bool SyncWriteFile(const std::filesystem::path& absolutePath, const TFileData& pData);
   const bool SyncAppendFile(const std::filesystem::path& absolutePath, const TFileData& pData);
   const bool SyncDeleteFile(const std::filesystem::path& absolutePath);
   void SyncGatherFiles(std::vector<std::filesystem::path>& filesFound, const std::filesystem::path& absolutePath);
   const std::string DataToString(const TFileData& pData);

};
