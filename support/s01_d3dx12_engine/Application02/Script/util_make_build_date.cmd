::util_make_build_date.cmd <1:date file output path> <2:build date stamp>
@ECHO OFF
PUSHD %~dp0

echo %~nx0 %~1 %~2

FOR /F "skip=1 tokens=1-6" %%G IN ('WMIC Path Win32_LocalTime Get Day^,Hour^,Minute^,Month^,Second^,Year /Format:table') DO (
   IF "%%~L"=="" goto S_DONE
      SET _yyyy=%%L
      SET _mm=00%%J
      SET _dd=00%%G
      SET _hour=00%%H
      SET _minute=00%%I
      SET _second=00%%K
)
: S_DONE

SET _mm=%_mm:~-2%
SET _dd=%_dd:~-2%
SET _hour=%_hour:~-2%
SET _minute=%_minute:~-2%
SET _second=%_second:~-2%

SET TIMESTAMP=%_yyyy%-%_mm%-%_dd%T%_hour%:%_minute%:%_second%
echo "%TIMESTAMP%"

> %1 echo "%TIMESTAMP%"

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "./copy_last_write_time.ps1" "%~1" "%~2"

POPD
exit /b %errorlevel%
