::prebuild_xbox.cmd <1:Project config> <2:Project platform> <3:Major version> <4:Minor version> <5:ProjectName> <6:TargetName>

::@ECHO OFF
PUSHD %~dp0

echo %time% %~nx0 %~1 %~2 %~3 %~4

SET DATA_DIR=..\source\

::SET LAYOUT_DIR=%~1

SET BUILD_DATE_FILE_PATH=%DATA_DIR%BuildTime.txt
SET BUILD_DESCRIPTION_FILE_PATH=%DATA_DIR%BuildDescription.txt
SET BUILD_VERSION_FILE_PATH=%DATA_DIR%BuildVersion.txt
SET BUILD_VERSION_FILE_PATH_0=%DATA_DIR%BuildVersionMajor.txt
SET BUILD_VERSION_FILE_PATH_1=%DATA_DIR%BuildVersionMinor.txt
SET BUILD_VERSION_FILE_PATH_2=%DATA_DIR%BuildVersionPatch.txt
SET BUILD_VERSION_FILE_PATH_3=%DATA_DIR%BuildVersionStore.txt

SET HOST_DEFINE_FILE_PATH=..\source\HostDefine.h

call util_make_build_date.cmd "%BUILD_DATE_FILE_PATH%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_make_build_description.cmd "%BUILD_DESCRIPTION_FILE_PATH%" "%~1" "%~2"
call util_make_build_description.cmd "%BUILD_DESCRIPTION_FILE_PATH%" "%~1" "%~2"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_make_build_version.cmd "%BUILD_VERSION_FILE_PATH%"
call util_make_build_version.cmd "%BUILD_VERSION_FILE_PATH%" "%~3" "%~4" "%BUILD_VERSION_FILE_PATH_0%" "%BUILD_VERSION_FILE_PATH_1%" "%BUILD_VERSION_FILE_PATH_2%" "%BUILD_VERSION_FILE_PATH_3%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_make_host_define.cmd "%HOST_DEFINE_FILE_PATH%"
call util_make_host_define.cmd "%HOST_DEFINE_FILE_PATH%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

::echo call util_make_resource.cmd "%BUILD_VERSION_FILE_PATH%"
call util_make_resource.cmd "%BUILD_VERSION_FILE_PATH%"
if %errorlevel% NEQ 0 (
   exit /b %ERRORLEVEL%
   )

echo %time% %~nx0 finished

POPD

exit /b %ERRORLEVEL%
