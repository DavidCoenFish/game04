::util_make_build_version.cmd <1:version file output path> <2:version file output path> <3:Major version> <4:Minor version> <5:version file 0 path> <6:version file 0 path> <7:version file 0 path> <8:version file 0 path> <9:build date stamp>
::util_make_build_version.cmd ..\source\BuildVersion.txt 0 0 ..\source\BuildVersionMajor.txt ..\source\BuildVersionMinor.txt ..\source\BuildVersionPatch.txt ..\source\BuildVersionStore.txt

::@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2 %~3 %~4 %~5 %~6 %~7 %~8 %~9

setlocal EnableDelayedExpansion

SET VERSION_PATCH=1
IF EXIST "%~7" (
   SET /P VERSION_PATCH=<"%~7"
   SET /A VERSION_PATCH=!VERSION_PATCH!+1
)

::(Get-Item "D:\Test\").LastWriteTime=("3 August 2019 17:00:00")

SET VERSION=%~3.%~4.%VERSION_PATCH%.0
SET VERSION_COMMA=%~3,%~4,%VERSION_PATCH%,0

echo "%VERSION%"

> %1 echo "%VERSION%"
> %2 echo %VERSION_COMMA%
> %5 echo %~3
> %6 echo %~4
> %7 echo %VERSION_PATCH%
> %8 echo 0

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~1" "%~9"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~2" "%~9"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~5" "%~9"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~6" "%~9"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~7" "%~9"
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~8" "%~9"

::copy_last_write_time.ps1 
::echo PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%1').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%1').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%2').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%5').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%6').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%7').LastWriteTime = (Get-Item '%~9').LastWriteTime"
::PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& (Get-Item '%8').LastWriteTime = (Get-Item '%~9').LastWriteTime"


if %errorlevel% NEQ 0 (
   echo swallow errorlevel %errorlevel%
   sort < nul > nul
   )

POPD
::exit /b %errorlevel%
exit /b 0
