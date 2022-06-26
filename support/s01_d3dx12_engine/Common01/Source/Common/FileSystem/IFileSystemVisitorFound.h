#pragma once

class FoundStaticFile;

class IFileSystemVisitorFound
{
public:
   typedef std::shared_ptr< std::vector< uint8_t > > TFileData;
   typedef std::function< void(void) > TVoidCallback;

   virtual ~IFileSystemVisitorFound();

   //found files/ folders
   //virtual void AddFoundStaticFile(FoundStaticFile* const pFoundStaticFile) = 0;
   //virtual void RemoveFoundStaticFile(FoundStaticFile* const pFoundStaticFile) = 0;
   virtual void AddAsyncTask(const TVoidCallback& callback) = 0;

};
