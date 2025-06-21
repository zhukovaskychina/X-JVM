@echo off
setlocal enabledelayedexpansion

echo ========================================
echo X-JVM vcpkg MinGW Setup Script
echo ========================================
echo.

REM 检查 MinGW 是否安装
where g++.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MinGW not found in PATH!
    echo [INFO] Please install MinGW-w64 or MSYS2 first
    pause
    exit /b 1
)

echo [OK] MinGW found

REM 检查 vcpkg 是否安装
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
        echo [ERROR] vcpkg not found!
        echo [INFO] Installing vcpkg...
        cd /d C:\
        git clone https://github.com/Microsoft/vcpkg.git
        cd vcpkg
        call bootstrap-vcpkg.bat
        set VCPKG_PATH=C:\vcpkg\vcpkg.exe
        echo [OK] vcpkg installed at C:\vcpkg\
    )
)

REM 安装 MinGW triplet 的依赖
echo [INFO] Installing Boost libraries for MinGW...
"%VCPKG_PATH%" install boost-system:x64-mingw-dynamic
"%VCPKG_PATH%" install boost-filesystem:x64-mingw-dynamic
"%VCPKG_PATH%" install boost-program-options:x64-mingw-dynamic
"%VCPKG_PATH%" install zlib:x64-mingw-dynamic

REM 集成 vcpkg
echo [INFO] Integrating vcpkg...
"%VCPKG_PATH%" integrate install

REM 设置环境变量
set VCPKG_ROOT=%VCPKG_PATH:vcpkg.exe=%
set VCPKG_ROOT=%VCPKG_ROOT:~0,-1%

echo [INFO] Setting environment variables:
echo   VCPKG_ROOT=%VCPKG_ROOT%

REM 创建环境变量设置脚本
echo @echo off > set_vcpkg_env.bat
echo set VCPKG_ROOT=%VCPKG_ROOT% >> set_vcpkg_env.bat
echo set CMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake >> set_vcpkg_env.bat
echo set VCPKG_TARGET_TRIPLET=x64-mingw-dynamic >> set_vcpkg_env.bat
echo echo [INFO] vcpkg environment variables set >> set_vcpkg_env.bat

echo [INFO] Created set_vcpkg_env.bat for future use

REM 返回项目目录
cd /d "%~dp0"

REM 清理现有构建
if exist "cmake-build-debug" (
    echo [INFO] Cleaning existing build directory...
    rmdir /s /q cmake-build-debug
)

REM 配置 CMake
echo [INFO] Configuring CMake build with vcpkg...
mkdir cmake-build-debug
cd cmake-build-debug

cmake .. ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic

if !errorlevel! neq 0 (
    echo [ERROR] CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo [OK] CMake configured successfully!

REM 编译项目
echo [INFO] Building X-JVM...
cmake --build . --config Debug

if !errorlevel! neq 0 (
    echo [ERROR] Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Build Summary
echo ========================================
echo [OK] vcpkg: Configured for MinGW
echo [OK] Boost: Installed via vcpkg (x64-mingw-dynamic)
echo [OK] X-JVM: Built successfully
echo.
echo [INFO] To set vcpkg environment variables in future sessions:
echo       call set_vcpkg_env.bat
echo.
echo [INFO] Executable location: cmake-build-debug\jvm.exe
echo ========================================

pause 