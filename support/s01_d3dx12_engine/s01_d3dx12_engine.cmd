pushd %~dp0
call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
::start "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv.exe" s01_d3dx12_engine.sln /edit
start devenv.exe s01_d3dx12_engine.sln
POPD
exit /b 0
