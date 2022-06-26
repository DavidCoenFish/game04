::util_make_build_description.cmd <1:description file output path> <2:Project config> <3:Project platform>

::@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2

FOR /F "usebackq" %%i IN (`hostname`) DO SET MACHINE_NAME=%%i

SET DESCRIPTION=%MACHINE_NAME% %~2 %~3

> %1 echo "%DESCRIPTION%"

POPD
exit /b %errorlevel%
