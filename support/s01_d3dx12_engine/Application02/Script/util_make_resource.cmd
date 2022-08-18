::util_make_resource <1:version file path>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1

SET /p VERSION=<"%~1"
SET VERSION=%VERSION:"=%
SET VERSION_COMMA=%VERSION:.=,%

call util_search_replace.cmd "..\Resources\Application02.rc.template" "..\Resources\Application02.rc.temp" "___FILE_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\Resources\Application02.rc.temp" "..\Resources\Application02.rc.temp" "___PRODUCT_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\Resources\Application02.rc.temp" "..\Resources\Application02.rc.temp" "___FILE_VERSION___" "%VERSION%"
call util_search_replace.cmd "..\Resources\Application02.rc.temp" "..\Resources\Application02.rc.temp" "___PRODUCT_VERSION___" "%VERSION%"

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./replace_if_different.ps1" "..\Resources\Application02.rc" "..\Resources\Application02.rc.temp"


POPD
exit /b %ERRORLEVEL%