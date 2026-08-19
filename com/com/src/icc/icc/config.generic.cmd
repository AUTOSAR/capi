rem @echo off


setlocal

set pwd=%~dp0
set src=%~dp0\%~1


rem check architecture
if not "%~2" == "" set a=%~2
if "%a%" == "" set a=all


rem is 32bit architecture
set b=0
if /I %a% == all set b=1
if /I %a% == x32 set b=1
if %b% neq 1 goto next

mkdir "%src%\building\x32"
cd "%src%\building\x32"
cmake %src% -G "Visual Studio 16 2019" -A Win32
cd %pwd%
if %errorlevel% neq 0 goto end


:next


rem is 64bit architecture
set b=0
if /I %a% == all set b=1
if /I %a% == x64 set b=1
if %b% neq 1 goto end

mkdir "%src%\building\x64"
cd "%src%\building\x64"
cmake %src% -G "Visual Studio 16 2019" -A x64
cd %pwd%
if %errorlevel% neq 0 goto end


:end

endlocal
exit /B