::util_make_build_version.cmd <1:version file output path> <2:version file output path> <3:Major version> <4:Minor version> <5:version file 0 path> <6:version file 0 path> <7:version file 0 path> <8:version file 0 path>
::util_make_build_version.cmd ..\source\BuildVersion.txt 0 0 ..\source\BuildVersionMajor.txt ..\source\BuildVersionMinor.txt ..\source\BuildVersionPatch.txt ..\source\BuildVersionStore.txt

::@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2 %~3 %~4 %~5 %~6 %~7 %~8

setlocal EnableDelayedExpansion

echo marco
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./update_version_if_hash_change.ps1" "%~7" "%~dp0"
echo polo

SET VERSION_PATCH=1
IF EXIST "%~7" (
   SET /P VERSION_PATCH=<"%~7"
)

::(Get-Item "D:\Test\").LastWriteTime=("3 August 2019 17:00:00")

SET VERSION=%~3.%~4.%VERSION_PATCH%.0
SET VERSION_COMMA=%~3,%~4,%VERSION_PATCH%,0

echo "%VERSION%"

> "%~1.temp" echo "%VERSION%"
> "%~2.temp" echo %VERSION_COMMA%
> "%~5.temp" echo %~3
> "%~6.temp" echo %~4
> "%~7.temp" echo %VERSION_PATCH%
> "%~8.temp" echo 0

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~1" "%~1.temp"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~2" "%~2.temp"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~5" "%~5.temp"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~6" "%~6.temp"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~7" "%~7.temp"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "%~8" "%~8.temp"

if %errorlevel% NEQ 0 (
   echo swallow errorlevel %errorlevel%
   sort < nul > nul
   )

POPD
::exit /b %errorlevel%
exit /b 0
