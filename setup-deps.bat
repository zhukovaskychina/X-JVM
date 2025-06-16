@echo off
setlocal enabledelayedexpansion

echo ================================
echo X-JVM Dependencies Setup Script
echo ================================
echo.

echo [INFO] This script will help you install the required dependencies for X-JVM
echo [INFO] Dependencies: CMake, Boost, zlib, C++ compiler
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] Running without administrator privileges.
    echo [WARNING] Some installations may fail.
    echo.
)

REM 检查包管理器
echo [INFO] Checking for package managers...

REM 检查 Chocolatey
choco --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Chocolatey found
    set HAVE_CHOCO=1
) else (
    echo [INFO] Chocolatey not found
    set HAVE_CHOCO=0
)

REM 检查 vcpkg
set VCPKG_PATH=
if exist "C:\vcpkg\vcpkg.exe" (
    set VCPKG_PATH=C:\vcpkg\vcpkg.exe
    echo [OK] vcpkg found at C:\vcpkg\
) else if exist "C:\tools\vcpkg\vcpkg.exe" (
    set VCPKG_PATH=C:\tools\vcpkg\vcpkg.exe
    echo [OK] vcpkg found at C:\tools\vcpkg\
) else (
    vcpkg --version >nul 2>&1
    if !errorlevel! equ 0 (
        set VCPKG_PATH=vcpkg
        echo [OK] vcpkg found in PATH
    ) else (
        echo [INFO] vcpkg not found
    )
)

echo.

REM 安装CMake
echo [INFO] Checking CMake...
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] CMake not found
    if %HAVE_CHOCO% equ 1 (
        echo [INFO] Installing CMake via Chocolatey...
        choco install cmake -y
    ) else (
        echo [MANUAL] Please install CMake from: https://cmake.org/download/
    )
) else (
    echo [OK] CMake found
)

REM 检查C++编译器
echo [INFO] Checking C++ compiler...

REM 检查Visual Studio
where cl.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Visual Studio compiler found
    set HAVE_MSVC=1
) else (
    echo [INFO] Visual Studio compiler not found in PATH
    set HAVE_MSVC=0
)

REM 检查MinGW
where g++.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] MinGW compiler found
    set HAVE_MINGW=1
) else (
    echo [INFO] MinGW compiler not found
    set HAVE_MINGW=0
)

if %HAVE_MSVC% equ 0 and %HAVE_MINGW% equ 0 (
    echo [WARNING] No C++ compiler found!
    if %HAVE_CHOCO% equ 1 (
        echo [INFO] You can install MinGW via: choco install mingw -y
    )
    echo [MANUAL] Or install Visual Studio from: https://visualstudio.microsoft.com/
)

echo.

REM 安装vcpkg (如果没有)
if "%VCPKG_PATH%"=="" (
    echo [INFO] vcpkg not found. Installing vcpkg...
    if exist "C:\" (
        cd /d C:\
        git clone https://github.com/Microsoft/vcpkg.git
        cd vcpkg
        call bootstrap-vcpkg.bat
        set VCPKG_PATH=C:\vcpkg\vcpkg.exe
        echo [OK] vcpkg installed at C:\vcpkg\
    ) else (
        echo [ERROR] Cannot install vcpkg automatically
        echo [MANUAL] Please install vcpkg manually:
        echo [MANUAL] 1. git clone https://github.com/Microsoft/vcpkg.git
        echo [MANUAL] 2. cd vcpkg
        echo [MANUAL] 3. .\bootstrap-vcpkg.bat
        goto :skip_vcpkg_install
    )
)

REM 安装C++依赖
if not "%VCPKG_PATH%"=="" (
    echo [INFO] Installing C++ libraries via vcpkg...
    
    echo [INFO] Installing Boost libraries...
    "%VCPKG_PATH%" install boost-system:x64-windows
    "%VCPKG_PATH%" install boost-filesystem:x64-windows
    "%VCPKG_PATH%" install boost-program-options:x64-windows
    
    echo [INFO] Installing zlib...
    "%VCPKG_PATH%" install zlib:x64-windows
    
    echo [INFO] Integrating vcpkg with Visual Studio...
    "%VCPKG_PATH%" integrate install
    
    echo [OK] Dependencies installed successfully!
    echo.
    echo [INFO] vcpkg toolchain file: %VCPKG_PATH:vcpkg.exe=%scripts\buildsystems\vcpkg.cmake
) else (
    echo [ERROR] Could not install dependencies via vcpkg
)

:skip_vcpkg_install

echo.
echo ================================
echo Setup Summary
echo ================================

REM 检查最终状态
cmake --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] CMake: Ready
) else (
    echo [FAIL] CMake: Not installed
)

if %HAVE_MSVC% equ 1 or %HAVE_MINGW% equ 1 (
    echo [OK] C++ Compiler: Ready
) else (
    echo [FAIL] C++ Compiler: Not found
)

if not "%VCPKG_PATH%"=="" (
    echo [OK] vcpkg: Ready
    echo [OK] Dependencies: Installed
) else (
    echo [FAIL] vcpkg: Not available
    echo [FAIL] Dependencies: Not installed
)

echo.
echo [INFO] To build the project, run:
echo [INFO]   build.bat
echo [INFO] or
echo [INFO]   .\build.ps1
echo.

pause 