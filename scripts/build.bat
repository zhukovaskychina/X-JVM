@echo off
setlocal enabledelayedexpansion

echo ================================
echo X-JVM Windows Build Script
echo ================================
echo.

REM 检查CMake是否安装
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found! Please install CMake and add it to PATH.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

REM 设置构建目录
set BUILD_DIR=build
set BUILD_TYPE=Release

REM 解析命令行参数
:parse_args
if "%~1"=="" goto :args_done
if "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)
if "%~1"=="--clean" (
    if exist %BUILD_DIR% (
        echo [INFO] Cleaning build directory...
        rmdir /s /q %BUILD_DIR%
    )
    shift
    goto :parse_args
)
if "%~1"=="--help" (
    echo Usage: build.bat [options]
    echo Options:
    echo   --debug    Build in Debug mode (default: Release)
    echo   --clean    Clean build directory before building
    echo   --help     Show this help message
    echo.
    pause
    exit /b 0
)
shift
goto :parse_args
:args_done

echo [INFO] Build type: %BUILD_TYPE%
echo.

REM 创建构建目录
if not exist %BUILD_DIR% (
    echo [INFO] Creating build directory...
    mkdir %BUILD_DIR%
)

REM 进入构建目录
cd %BUILD_DIR%

echo [INFO] Running CMake configuration...

REM 构建CMake命令，优先使用vcpkg
set CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%

REM 检查是否有vcpkg
set VCPKG_TOOLCHAIN=
if exist "C:\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    set VCPKG_TOOLCHAIN=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
    echo [INFO] Using vcpkg toolchain: !VCPKG_TOOLCHAIN!
) else if exist "C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake" (
    set VCPKG_TOOLCHAIN=C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake
    echo [INFO] Using vcpkg toolchain: !VCPKG_TOOLCHAIN!
) else if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
        set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
        echo [INFO] Using vcpkg toolchain: !VCPKG_TOOLCHAIN!
    )
)

if not "!VCPKG_TOOLCHAIN!"=="" (
    set CMAKE_ARGS=!CMAKE_ARGS! -DCMAKE_TOOLCHAIN_FILE="!VCPKG_TOOLCHAIN!"
)

REM 运行CMake配置
cmake .. %CMAKE_ARGS%
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    echo.
    echo [SOLUTION] This is likely due to missing Boost libraries. Try these solutions:
    echo.
    echo 1. Install dependencies using vcpkg:
    echo    setup-deps.bat
    echo.
    echo 2. Or manually install vcpkg and dependencies:
    echo    git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    echo    C:\vcpkg\bootstrap-vcpkg.bat
    echo    C:\vcpkg\vcpkg install boost-system boost-filesystem boost-program-options zlib --triplet x64-windows
    echo    C:\vcpkg\vcpkg integrate install
    echo.
    echo 3. Or set BOOST_ROOT to your Boost installation:
    echo    set BOOST_ROOT=C:\path\to\boost
    echo    build.bat
    echo.
    echo 4. For MinGW users, install Boost via MSYS2:
    echo    pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-zlib
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building project...
cmake --build . --config %BUILD_TYPE%
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo [SUCCESS] Build completed successfully!
echo [INFO] Executable location: %BUILD_DIR%\%BUILD_TYPE%\jvm.exe
echo.

REM 检查可执行文件是否存在
if exist "%BUILD_DIR%\%BUILD_TYPE%\jvm.exe" (
    echo [INFO] Testing executable...
    "%BUILD_DIR%\%BUILD_TYPE%\jvm.exe" --help
) else if exist "%BUILD_DIR%\jvm.exe" (
    echo [INFO] Testing executable...
    "%BUILD_DIR%\jvm.exe" --help
) else (
    echo [WARNING] Executable not found in expected location.
)

echo.
echo Build script completed.
pause 