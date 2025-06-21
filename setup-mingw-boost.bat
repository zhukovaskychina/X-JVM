@echo off
setlocal enabledelayedexpansion

echo ========================================
echo X-JVM MinGW Boost Setup Script
echo ========================================
echo.

REM 检查 MinGW 是否安装
where g++.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MinGW not found in PATH!
    echo [INFO] Please install MinGW-w64 or MSYS2 first
    echo [INFO] MSYS2: https://www.msys2.org/
    echo [INFO] MinGW-w64: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

echo [OK] MinGW found
g++ --version | findstr "g++"

REM 检查是否有现有的 Boost
set BOOST_DIR=C:\boost-mingw
if exist "%BOOST_DIR%" (
    echo [INFO] Found existing Boost at %BOOST_DIR%
    choice /c YN /m "Do you want to rebuild Boost? (Y/N)"
    if !errorlevel! equ 2 goto :configure_cmake
)

REM 创建构建目录
if not exist "%BOOST_DIR%" mkdir "%BOOST_DIR%"
cd /d "%BOOST_DIR%"

REM 下载 Boost (如果不存在)
set BOOST_VERSION=1.82.0
set BOOST_VERSION_UNDERSCORE=1_82_0
set BOOST_ARCHIVE=boost_%BOOST_VERSION_UNDERSCORE%.zip
set BOOST_URL=https://boostorg.jfrog.io/artifactory/main/release/%BOOST_VERSION%/source/%BOOST_ARCHIVE%

if not exist "boost_%BOOST_VERSION_UNDERSCORE%" (
    echo [INFO] Downloading Boost %BOOST_VERSION%...
    if not exist "%BOOST_ARCHIVE%" (
        powershell -Command "Invoke-WebRequest -Uri '%BOOST_URL%' -OutFile '%BOOST_ARCHIVE%'"
    )
    
    echo [INFO] Extracting Boost...
    powershell -Command "Expand-Archive -Path '%BOOST_ARCHIVE%' -DestinationPath '.'"
)

cd boost_%BOOST_VERSION_UNDERSCORE%

REM 配置 Boost.Build
echo [INFO] Configuring Boost.Build for MinGW...
if not exist "b2.exe" (
    call bootstrap.bat gcc
    if !errorlevel! neq 0 (
        echo [ERROR] Failed to bootstrap Boost.Build
        pause
        exit /b 1
    )
)

REM 编译 Boost 库
echo [INFO] Building Boost libraries with MinGW...
echo [INFO] This may take 10-30 minutes depending on your system...

.\b2.exe ^
    toolset=gcc ^
    --with-system ^
    --with-filesystem ^
    --with-program_options ^
    link=shared,static ^
    runtime-link=shared ^
    threading=multi ^
    variant=release,debug ^
    address-model=64 ^
    --prefix="%BOOST_DIR%" ^
    install

if !errorlevel! neq 0 (
    echo [ERROR] Failed to build Boost
    pause
    exit /b 1
)

echo [OK] Boost built successfully!

:configure_cmake
echo.
echo [INFO] Configuring CMake for X-JVM...

REM 返回项目目录
cd /d "%~dp0"

REM 设置环境变量
set BOOST_ROOT=%BOOST_DIR%
set BOOST_LIBRARYDIR=%BOOST_DIR%\lib
set BOOST_INCLUDEDIR=%BOOST_DIR%\include

echo [INFO] Setting environment variables:
echo   BOOST_ROOT=%BOOST_ROOT%
echo   BOOST_LIBRARYDIR=%BOOST_LIBRARYDIR%
echo   BOOST_INCLUDEDIR=%BOOST_INCLUDEDIR%

REM 创建环境变量设置脚本
echo @echo off > set_boost_env.bat
echo set BOOST_ROOT=%BOOST_DIR% >> set_boost_env.bat
echo set BOOST_LIBRARYDIR=%BOOST_DIR%\lib >> set_boost_env.bat
echo set BOOST_INCLUDEDIR=%BOOST_DIR%\include >> set_boost_env.bat
echo echo [INFO] Boost environment variables set >> set_boost_env.bat

echo [INFO] Created set_boost_env.bat for future use

REM 清理现有构建
if exist "cmake-build-debug" (
    echo [INFO] Cleaning existing build directory...
    rmdir /s /q cmake-build-debug
)

REM 配置 CMake
echo [INFO] Configuring CMake build...
mkdir cmake-build-debug
cd cmake-build-debug

cmake .. ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DBOOST_ROOT="%BOOST_DIR%" ^
    -DBoost_USE_STATIC_LIBS=OFF ^
    -DBoost_USE_MULTITHREADED=ON ^
    -DBoost_USE_STATIC_RUNTIME=OFF

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
echo [OK] Boost: Built with MinGW at %BOOST_DIR%
echo [OK] X-JVM: Built successfully
echo.
echo [INFO] To set Boost environment variables in future sessions:
echo       call set_boost_env.bat
echo.
echo [INFO] Executable location: cmake-build-debug\jvm.exe
echo ========================================

pause 