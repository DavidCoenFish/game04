#include "ApplicationPCH.h"
#include "Build.h"

/*
   including txt files without the build marking itself dirty consecutively seems to work?
   want some way of getting when the build was done into the build with out dirtying the build and forcing a rebuild....
*/

const char* const Build::GetBuildDescription( void )
{
   return 
#include "..\..\Version00\Output\BuildHost.txt"
//       " "
//#include "..\..\Version00\Output\BuildHash.txt"
#if defined(_DEBUG)
       " DEBUG"
#endif
#if defined(NDEBUG)
       " RELEASE"
#endif
       ;
}
const char* const Build::GetBuildTime( void )
{
   return 
       __DATE__ 
       " "
       __TIME__
       ;
}
const char* const Build::GetBuildVersion( void )
{
   return 
#include "..\..\Version00\Output\BuildVersion.txt"
       ;
}

const int Build::GetBuildVersionMajor()
{
   return 
#include "..\..\Version00\Output\BuildVersionMajor.txt"
      ;
}

const int Build::GetBuildVersionMinor()
{
   return 
#include "..\..\Version00\Output\BuildVersionMinor.txt"
      ;
}

const int Build::GetBuildVersionPatch()
{
   return 
#include "..\..\Version00\Output\BuildVersionPatch.txt"
      ;
}
const int Build::GetBuildVersionStore()
{
   return 
#include "..\..\Version00\Output\BuildVersionStore.txt"
      ;
}


