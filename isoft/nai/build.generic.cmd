rem @echo off


setlocal


set src=%~dp0\%~1

rem check architecture
if not "%~3" == "" set a=%~3
if "%a%" == "" set a=all


rem check configuration
if not "%~4" == "" set m=%~4
if "%m%" == "" set m=all


if not "%~2" == "" set pro=%~2
if "%pro%" == "" set pro="all_build"


call "%~dp0\config.generic.cmd" %1 %a%
if %errorlevel% neq 0 goto end



rem is 32bit architecture
set b=0
if /I %a% == all set b=1
if /I %a% == x32 set b=1
if %b% neq 1 goto next


set c=0
if /I %m% == all set c=1
if /I %m% == debug set c=1
if %c% == 1 cmake --build "%src%\building\x32" --config Debug --target %pro%
if %errorlevel% neq 0 goto end

set c=0
if /I %m% == all set c=1
if /I %m% == release set c=1
if %c% == 1 cmake --build "%src%\building\x32" --config Release --target %pro%
if %errorlevel% neq 0 goto end


:next


rem is 64bit architecture
set b=0
if /I %a% == all set b=1
if /I %a% == x64 set b=1
if %b% neq 1 goto end


set c=0
if /I %m% == all set c=1
if /I %m% == debug set c=1
if %c% == 1 cmake --build "%src%\building\x64" --config Debug --target %pro%
if %errorlevel% neq 0 goto end

set c=0
if /I %m% == all set c=1
if /I %m% == release set c=1
if %c% == 1 cmake --build "%src%\building\x64" --config Release --target %pro%
if %errorlevel% neq 0 goto end


:end

endlocal
exit /B