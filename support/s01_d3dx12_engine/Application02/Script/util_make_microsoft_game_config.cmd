::util_make_microsoft_game_config <1:version file path>  <2:Project config> <3:Project platform> <4:ProjectName> <5:TargetName>

@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2

SET /p REPLACE_VERSION=<"%~1"
SET REPLACE_VERSION=%REPLACE_VERSION:"=%

call util_search_replace.cmd "..\MicrosoftGame.Config.template" "..\MicrosoftGame.Config" "__IDENTITY_VERSION__" "%REPLACE_VERSION%"

POPD
exit /b %ERRORLEVEL%