@ECHO OFF
PUSHD %~dp0

cls

echo %time% %~nx0 %~1

SET MISSING_AUTOMATRON=
IF NOT EXIST "%~dp0..\..\build\Automatron03\Release\bin\net5.0\Automatron03.exe" SET MISSING_AUTOMATRON=TRUE
IF NOT EXIST "%~dp0..\..\build\Automatron03\Release\bin\net5.0\Automatron03.dll" SET MISSING_AUTOMATRON=TRUE

IF defined MISSING_AUTOMATRON (
   echo "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com" "%~dp0s01_d3dx12_engine.sln" /build "Release|Gaming.Desktop.x64" /project Automatron03
   call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com" "%~dp0s01_d3dx12_engine.sln" /build "Release|Gaming.Desktop.x64" /project Automatron03
   )

echo "%~dp0..\..\build\s01_d3dx12_engine\Automatron03\Debug\bin\netcoreapp3.1\Automatron03.exe" %~dp0 %~dp0Automatron03\TasksTest
call "%~dp0..\..\build\s01_d3dx12_engine\Automatron03\Debug\bin\netcoreapp3.1\Automatron03.exe" %~dp0 %~dp0Automatron03\TasksTest
if %errorlevel% NEQ 0 (
   echo Automatron03.exe exited with %errorlevel%
   exit /b %errorlevel%
   )

POPD

exit /b 0


