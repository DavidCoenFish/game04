::util_make_resource <1:version file path>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1

SET /p VERSION=<"%~1"
SET VERSION=%VERSION:"=%
SET VERSION_COMMA=%VERSION:.=,%

call util_search_replace.cmd "..\Application01.rc.template" "..\Application01.rc" "___FILE_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\Application01.rc" "..\Application01.rc" "___PRODUCT_VERSION_COMMA___" "%VERSION_COMMA%"
call util_search_replace.cmd "..\Application01.rc" "..\Application01.rc" "___FILE_VERSION___" "%VERSION%"
call util_search_replace.cmd "..\Application01.rc" "..\Application01.rc" "___PRODUCT_VERSION___" "%VERSION%"

POPD
exit /b %ERRORLEVEL%