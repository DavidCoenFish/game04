#include "ApplicationPCH.h"
#include "Build.h"

/*
   including txt files without the build marking itself dirty consecutively seems to work?
   want some way of getting when the build was done into the build with out dirtying the build and forcing a rebuild....
*/

const char* const Build::GetBuildTime( void )
{
   return 
#include "BuildTime.txt"
      "\n";
}
const char* const Build::GetBuildVersion( void )
{
   return 
#include "BuildVersion.txt"
      "\n";
}
const char* const Build::GetBuildDescription( void )
{
   return 
#include "BuildDescription.txt"
      "\n";
}

const int Build::GetBuildVersionMajor()
{
   return 
#include "BuildVersionMajor.txt"
      ;
}

const int Build::GetBuildVersionMinor()
{
   return 
#include "BuildVersionMinor.txt"
      ;
}

const int Build::GetBuildVersionPatch()
{
   return 
#include "BuildVersionPatch.txt"
      ;
}
const int Build::GetBuildVersionStore()
{
   return 
#include "BuildVersionStore.txt"
      ;
}


