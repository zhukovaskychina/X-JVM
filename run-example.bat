@echo off
setlocal enabledelayedexpansion

echo ================================
echo X-JVM Example Runner
echo ================================
echo.

REM 检查可执行文件是否存在
set JVM_EXE=
if exist "build\Release\jvm.exe" (
    set JVM_EXE=build\Release\jvm.exe
) else if exist "build\Debug\jvm.exe" (
    set JVM_EXE=build\Debug\jvm.exe
) else if exist "build\jvm.exe" (
    set JVM_EXE=build\jvm.exe
) else (
    echo [ERROR] JVM executable not found!
    echo [INFO] Please build the project first using:
    echo [INFO]   build.bat
    echo [INFO] or
    echo [INFO]   .\build.ps1
    pause
    exit /b 1
)

echo [INFO] Found JVM executable: %JVM_EXE%
echo.

REM 检查是否提供了参数
if "%~1"=="" (
    echo [INFO] Usage: run-example.bat [java-class-file] [jre-path] [classpath]
    echo [INFO] 
    echo [INFO] Example:
    echo [INFO]   run-example.bat Hello.class "C:\Program Files\Java\jdk-11\jre" "."
    echo [INFO]   run-example.bat Test.class "C:\Java\jre" "C:\MyProject\classes"
    echo.
    echo [INFO] If no arguments provided, will show JVM help:
    echo.
    "%JVM_EXE%" --help
    echo.
    pause
    exit /b 0
)

REM 解析参数
set JAVA_CLASS=%~1
set JRE_PATH=%~2
set CLASS_PATH=%~3

REM 设置默认值
if "%JRE_PATH%"=="" (
    REM 尝试自动检测JRE路径
    set JRE_PATH=
    if exist "C:\Program Files\Java\jdk-11\jre" (
        set JRE_PATH=C:\Program Files\Java\jdk-11\jre
    ) else if exist "C:\Program Files\Java\jdk-17\jre" (
        set JRE_PATH=C:\Program Files\Java\jdk-17\jre
    ) else if exist "C:\Program Files\Java\jdk-8\jre" (
        set JRE_PATH=C:\Program Files\Java\jdk-8\jre
    )
    
    if "!JRE_PATH!"=="" (
        echo [ERROR] JRE path not specified and auto-detection failed!
        echo [INFO] Please specify JRE path as second argument
        echo [INFO] Example: run-example.bat Hello.class "C:\Program Files\Java\jdk-11\jre"
        pause
        exit /b 1
    ) else (
        echo [INFO] Auto-detected JRE: !JRE_PATH!
    )
)

if "%CLASS_PATH%"=="" (
    set CLASS_PATH=.
    echo [INFO] Using default classpath: .
)

REM 检查Java类文件是否存在
if not exist "%JAVA_CLASS%" (
    echo [ERROR] Java class file not found: %JAVA_CLASS%
    echo [INFO] Please make sure the .class file exists
    pause
    exit /b 1
)

REM 检查JRE路径是否存在
if not exist "%JRE_PATH%" (
    echo [ERROR] JRE path not found: %JRE_PATH%
    echo [INFO] Please check your JRE installation
    pause
    exit /b 1
)

echo [INFO] Running X-JVM with:
echo [INFO]   Java Class: %JAVA_CLASS%
echo [INFO]   JRE Path: %JRE_PATH%
echo [INFO]   Classpath: %CLASS_PATH%
echo.

REM 运行JVM
echo [INFO] Executing: "%JVM_EXE%" --xjre "%JRE_PATH%" --classpath "%CLASS_PATH%" --javaclass "%JAVA_CLASS%"
echo ================================
"%JVM_EXE%" --xjre "%JRE_PATH%" --classpath "%CLASS_PATH%" --javaclass "%JAVA_CLASS%"
set EXIT_CODE=%errorlevel%
echo ================================

if %EXIT_CODE% equ 0 (
    echo [SUCCESS] JVM execution completed successfully!
) else (
    echo [ERROR] JVM execution failed with exit code: %EXIT_CODE%
)

echo.
pause 