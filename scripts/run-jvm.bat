@echo off
echo ===== X-JVM 运行脚本 =====
echo.

REM 设置环境变量
set PATH=C:\Users\Administrator\vcpkg\installed\x64-mingw-dynamic\bin;%PATH%
set PATH=C:\ProgramData\mingw64\mingw64\bin;%PATH%

REM 检查可执行文件是否存在
if not exist "build\jvm.exe" (
    echo 错误: build\jvm.exe 不存在
    echo 请先运行 build.bat 编译项目
    pause
    exit /b 1
)

echo 当前工作目录: %CD%
echo.

REM 显示使用说明
if "%1"=="" (
    echo 使用方法:
    echo   run-jvm.bat --help              显示帮助
    echo   run-jvm.bat --xjre ^<jre路径^> --classpath ^<类路径^> --javaclass ^<类名^>
    echo.
    echo 示例:
    echo   run-jvm.bat --xjre "C:\Program Files\Java\jdk-11\jre" --classpath "." --javaclass "HelloWorld.class"
    echo.
    echo 尝试显示帮助信息...
    build\jvm.exe --help
) else (
    echo 运行 JVM 参数: %*
    build\jvm.exe %*
)

echo.
echo 程序执行完毕 (退出码: %errorlevel%)
if not "%1"=="--help" pause 