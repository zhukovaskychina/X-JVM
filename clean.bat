@echo off
setlocal

echo ================================
echo X-JVM Clean Script
echo ================================
echo.

echo [INFO] This script will clean build artifacts and temporary files.
echo.

REM 询问用户确认
set /p confirm="Are you sure you want to clean the project? (y/N): "
if /i not "%confirm%"=="y" (
    echo [INFO] Clean operation cancelled.
    pause
    exit /b 0
)

echo.
echo [INFO] Cleaning build directory...
if exist "build" (
    rmdir /s /q "build"
    echo [OK] Removed build directory
) else (
    echo [INFO] Build directory not found
)

echo [INFO] Cleaning CMake cache files...
if exist "CMakeCache.txt" (
    del "CMakeCache.txt"
    echo [OK] Removed CMakeCache.txt
)

if exist "CMakeFiles" (
    rmdir /s /q "CMakeFiles"
    echo [OK] Removed CMakeFiles directory
)

echo [INFO] Cleaning Visual Studio files...
if exist "*.vcxproj*" (
    del "*.vcxproj*"
    echo [OK] Removed Visual Studio project files
)

if exist "*.sln" (
    del "*.sln"
    echo [OK] Removed Visual Studio solution files
)

if exist ".vs" (
    rmdir /s /q ".vs"
    echo [OK] Removed .vs directory
)

echo [INFO] Cleaning temporary files...
if exist "*.tmp" (
    del "*.tmp"
    echo [OK] Removed temporary files
)

if exist "*.log" (
    del "*.log"
    echo [OK] Removed log files
)

echo.
echo [SUCCESS] Project cleaned successfully!
echo [INFO] You can now run build.bat or build.ps1 to rebuild the project.
echo.

pause 