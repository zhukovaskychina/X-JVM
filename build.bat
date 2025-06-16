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
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
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