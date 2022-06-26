#pragma once

class IFileSystemProvider;

class IFileSystemVisitorProvider
{
public:
   virtual ~IFileSystemVisitorProvider();

   //provider
   virtual void OnReady(const IFileSystemProvider* pProvider) = 0;
   //virtual void OnStaticFilesChange(IFileSystemProvider* const pProvider) = 0;

};
