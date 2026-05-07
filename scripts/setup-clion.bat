@echo off
setlocal enabledelayedexpansion

echo ================================
echo X-JVM CLion 快速配置脚本
echo ================================
echo.

REM 检查CLion是否在运行
tasklist /FI "IMAGENAME eq clion64.exe" 2>NUL | find /I /N "clion64.exe">NUL
if %errorlevel% == 0 (
    echo [警告] CLion 正在运行，建议先关闭 CLion 后再运行此脚本
    echo.
)

REM 检查vcpkg是否存在
if not exist "C:\vcpkg\vcpkg.exe" (
    echo [错误] 未找到 vcpkg，请先运行 setup-deps.bat
    pause
    exit /b 1
)

echo [INFO] 找到 vcpkg 安装...

REM 验证依赖是否已安装
echo [INFO] 检查依赖安装状态...
C:\vcpkg\vcpkg.exe list > vcpkg_list.tmp

findstr /C:"boost-filesystem:x64-windows" vcpkg_list.tmp >nul
if %errorlevel% neq 0 (
    echo [警告] boost-filesystem 未安装，正在安装...
    C:\vcpkg\vcpkg.exe install boost-filesystem --triplet x64-windows
)

findstr /C:"boost-program-options:x64-windows" vcpkg_list.tmp >nul
if %errorlevel% neq 0 (
    echo [警告] boost-program-options 未安装，正在安装...
    C:\vcpkg\vcpkg.exe install boost-program-options --triplet x64-windows
)

findstr /C:"boost-system:x64-windows" vcpkg_list.tmp >nul
if %errorlevel% neq 0 (
    echo [警告] boost-system 未安装，正在安装...
    C:\vcpkg\vcpkg.exe install boost-system --triplet x64-windows
)

findstr /C:"zlib:x64-windows" vcpkg_list.tmp >nul
if %errorlevel% neq 0 (
    echo [警告] zlib 未安装，正在安装...
    C:\vcpkg\vcpkg.exe install zlib --triplet x64-windows
)

del vcpkg_list.tmp

echo [INFO] 验证 vcpkg 集成...
C:\vcpkg\vcpkg.exe integrate install

REM 清理旧的构建文件
echo [INFO] 清理旧的构建文件...
if exist "cmake-build-debug" (
    rmdir /s /q "cmake-build-debug"
    echo [INFO] 已清理 cmake-build-debug
)

if exist "cmake-build-release" (
    rmdir /s /q "cmake-build-release"
    echo [INFO] 已清理 cmake-build-release
)

if exist ".idea\cmake.xml" (
    del ".idea\cmake.xml"
    echo [INFO] 已清理 CLion CMake 缓存
)

REM 创建环境变量设置脚本
echo [INFO] 创建环境变量设置...
echo @echo off > set_env.bat
echo set VCPKG_ROOT=C:\vcpkg >> set_env.bat
echo set CMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake >> set_env.bat
echo set VCPKG_TARGET_TRIPLET=x64-windows >> set_env.bat
echo echo [INFO] 环境变量已设置 >> set_env.bat

REM 测试CMake配置
echo [INFO] 测试 CMake 配置...
mkdir test_build 2>nul
cd test_build

cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -G "MinGW Makefiles" > cmake_test.log 2>&1

if %errorlevel% == 0 (
    echo [成功] CMake 配置测试通过！
    
    REM 检查是否找到了Boost
    findstr /C:"Found Boost" cmake_test.log >nul
    if %errorlevel% == 0 (
        echo [成功] Boost 库已找到
    ) else (
        echo [警告] Boost 库可能未正确找到
    )
    
    REM 检查是否找到了zlib
    findstr /C:"Found.*zlib" cmake_test.log >nul
    if %errorlevel% == 0 (
        echo [成功] zlib 库已找到
    ) else (
        echo [警告] zlib 库可能未正确找到
    )
    
) else (
    echo [错误] CMake 配置测试失败，请查看 test_build\cmake_test.log
)

cd ..
rmdir /s /q test_build

echo.
echo ================================
echo CLion 配置指南
echo ================================
echo.
echo 1. 打开 CLion
echo 2. 打开设置 (Ctrl+Alt+S)
echo 3. 导航到: Build, Execution, Deployment → CMake
echo 4. 在 "CMake options" 中添加:
echo    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
echo    -DVCPKG_TARGET_TRIPLET=x64-windows
echo.
echo 5. 点击 "Apply" 然后 "OK"
echo 6. 在 CLion 中: Tools → CMake → Reset Cache and Reload Project
echo.
echo ================================
echo 环境变量设置 (可选)
echo ================================
echo 如果需要设置环境变量，运行: set_env.bat
echo.

echo [完成] CLion 配置准备完成！
echo.
echo 接下来请:
echo 1. 按照上述指南配置 CLion
echo 2. 重新加载项目
echo 3. 尝试构建项目
echo.
pause 