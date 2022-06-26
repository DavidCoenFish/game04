::util_make_build_version.cmd <1:version file output path> <2:Major version> <3:Minor version> <4:version file 0 path> <5:version file 0 path> <6:version file 0 path> <7:version file 0 path> 
::util_make_build_version.cmd ..\source\BuildVersion.txt 0 0 ..\source\BuildVersionMajor.txt ..\source\BuildVersionMinor.txt ..\source\BuildVersionPatch.txt ..\source\BuildVersionStore.txt

::@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2 %~3 %~4 %~5 %~6 %~7

setlocal EnableDelayedExpansion

SET VERSION_PATCH=1
IF EXIST "%~6" (
   SET /P VERSION_PATCH=<"%~6"
   set /A VERSION_PATCH=!VERSION_PATCH!+1
)

SET VERSION=%~2.%~3.%VERSION_PATCH%.0

> %1 echo "%VERSION%"
> %4 echo %~2
> %5 echo %~3
> %6 echo %VERSION_PATCH%
> %7 echo 0

if %errorlevel% NEQ 0 (
   echo swallow errorlevel %errorlevel%
   sort < nul > nul
   )

POPD
::exit /b %errorlevel%
exit /b 0
