@echo off
setlocal enabledelayedexpansion

echo ======================================
echo  Unreal Project Full Rebuild Script
echo ======================================

:: --------------------------------------
:: CONFIGURATION
:: --------------------------------------
:: Chemin vers Unreal Engine (modifier si besoin)
set UE_ROOT=E:\Games\UE_5.6

:: --------------------------------------
:: DETECTION DU PROJET
:: --------------------------------------
set PROJECT_DIR=%~dp0
for %%f in ("%PROJECT_DIR%*.uproject") do (
    set UPROJECT=%%f
    set PROJECT_NAME=%%~nf
)

if not defined UPROJECT (
    echo [ERROR] No .uproject found in this folder!
    pause
    exit /b 1
)

echo Project detected: %PROJECT_NAME%
echo Project path: %UPROJECT%

:: --------------------------------------
:: WAIT FOR UNREAL EDITOR
:: --------------------------------------
echo.
echo Waiting for Unreal Editor to close...
:wait_for_editor
tasklist /FI "IMAGENAME eq UnrealEditor.exe" | find /I "UnrealEditor.exe" >nul
if %errorlevel%==0 (
    timeout /t 2 >nul
    goto wait_for_editor
)
echo Unreal Editor closed.

:: --------------------------------------
:: CLEAN BUILD FILES
:: --------------------------------------
echo.
echo Deleting old build folders...
rmdir /s /q "%PROJECT_DIR%Binaries" 2>nul
rmdir /s /q "%PROJECT_DIR%Intermediate" 2>nul
del /q "%PROJECT_DIR%*.sln" 2>nul

:: --------------------------------------
:: UNREAL BUILD TOOL
:: --------------------------------------
set UBT_PATH=%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
if not exist "%UBT_PATH%" (
    set UBT_PATH=%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool.exe
)

if not exist "%UBT_PATH%" (
    echo [ERROR] UnrealBuildTool.exe not found!
    pause
    exit /b 1
)

:: --------------------------------------
:: GENERATE PROJECT FILES
:: --------------------------------------
echo.
echo Regenerating Visual Studio project files...
"%UBT_PATH%" -projectfiles -project="%UPROJECT%" -game -engine -progress
if %errorlevel% neq 0 (
    echo [ERROR] Failed to generate project files.
    pause
    exit /b %errorlevel%
)

:: --------------------------------------
:: BUILD PROJECT
:: --------------------------------------
echo.
echo Compiling project...
"%UBT_PATH%" %PROJECT_NAME%Editor Win64 Development -project="%UPROJECT%" -progress
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %errorlevel%
)

:: --------------------------------------
:: LAUNCH EDITOR
:: --------------------------------------
echo.
echo Launching Unreal Editor...
start "" "%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%UPROJECT%" -Build

echo.
echo All tasks completed successfully!
pause
endlocal
