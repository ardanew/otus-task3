@echo off
setlocal EnableDelayedExpansion && pushd .& set RET=0

:: ---------- Init
:DETECT_HW3_DEPS
if not defined HW3_DEPS ( if NOT exist %CD%\scripts\win (cd .. & goto :DETECT_HW3_DEPS) else (set HW3_DEPS=%CD%) )

cd %~dp0\..

call scripts\colors_on.bat
call scripts\locate_vs2022.bat
if %RET% NEQ 0 (goto :EXITCMD)

set PLATFORM_BUILD_DIR=x64-win-vs-release
call scripts\cleanup.bat build\%PLATFORM_BUILD_DIR%
set INSTALL_TO=%CD%\output\%PLATFORM_BUILD_DIR%
call scripts\cleanup.bat %INSTALL_TO%
set BUILD_TYPE=Release

:: -------- CMake
echo *%C_YELLOW% Run cmake %PLATFORM_BUILD_DIR% %C_DEF%
cmake -DCMAKE_INSTALL_PREFIX=%INSTALL_TO% -DCMAKE_GENERATOR_PLATFORM=x64^
 -DCMAKE_BUILD_TYPE=Release^
 -B build/%PLATFORM_BUILD_DIR% -S .^
 -DPLATFORM_BUILD_DIR=%PLATFORM_BUILD_DIR%^
 -DBUILD_TESTS=ON^
 -G %VS_GENERATOR%
if ERRORLEVEL 1 ( set RET=104& goto :EXITCMD )
echo.

:: -------- Build
echo *%C_YELLOW% Building %PLATFORM_BUILD_DIR% %C_DEF%
set PreferredToolArchitecture=x64
cmake --build build/%PLATFORM_BUILD_DIR% -- /property:Configuration=Release
if ERRORLEVEL 1 ( set RET=105& goto :EXITCMD )
echo.

:: -------- Install
echo *%C_YELLOW% Installing %PLATFORM_BUILD_DIR% %C_DEF%
cmake --install build/%PLATFORM_BUILD_DIR% --config=Release
if ERRORLEVEL 1 ( set RET=106& goto :EXITCMD )
echo.

:: ---------- Exit
:EXITCMD
POPD
if !RET! NEQ 0 (
  echo %C_RED% Error: %SCRIPTNAME% failed with code %RET% %C_DEF%
)
exit /b %RET%
