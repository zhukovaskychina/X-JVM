# X-JVM Windows PowerShell Build Script
param(
    [string]$BuildType = "Release",
    [switch]$Clean,
    [switch]$Help,
    [switch]$InstallDeps,
    [string]$Generator = "",
    [switch]$Verbose
)

# 显示帮助信息
if ($Help) {
    Write-Host "X-JVM Windows Build Script" -ForegroundColor Green
    Write-Host "Usage: .\build.ps1 [options]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Options:" -ForegroundColor Cyan
    Write-Host "  -BuildType <type>    Build type: Release, Debug, RelWithDebInfo, MinSizeRel (default: Release)"
    Write-Host "  -Clean               Clean build directory before building"
    Write-Host "  -InstallDeps         Install dependencies using vcpkg"
    Write-Host "  -Generator <gen>     CMake generator (e.g., 'Visual Studio 16 2019', 'MinGW Makefiles')"
    Write-Host "  -Verbose             Enable verbose output"
    Write-Host "  -Help                Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Magenta
    Write-Host "  .\build.ps1                                    # Release build"
    Write-Host "  .\build.ps1 -BuildType Debug                   # Debug build"
    Write-Host "  .\build.ps1 -Clean -BuildType Release          # Clean and build"
    Write-Host "  .\build.ps1 -InstallDeps                       # Install dependencies first"
    Write-Host "  .\build.ps1 -Generator 'MinGW Makefiles'       # Use MinGW"
    exit 0
}

Write-Host "================================" -ForegroundColor Green
Write-Host "X-JVM Windows Build Script" -ForegroundColor Green
Write-Host "================================" -ForegroundColor Green
Write-Host ""

# 检查CMake
Write-Host "[INFO] Checking CMake..." -ForegroundColor Blue
try {
    $cmakeVersion = cmake --version 2>$null
    Write-Host "[OK] CMake found: $($cmakeVersion[0])" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] CMake not found! Please install CMake and add it to PATH." -ForegroundColor Red
    Write-Host "Download from: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# 设置变量
$BuildDir = "build"
$ProjectRoot = Get-Location

Write-Host "[INFO] Build type: $BuildType" -ForegroundColor Blue

# 安装依赖
if ($InstallDeps) {
    Write-Host "[INFO] Installing dependencies..." -ForegroundColor Blue
    
    # 检查vcpkg
    $vcpkgPath = ""
    if (Test-Path "C:\vcpkg\vcpkg.exe") {
        $vcpkgPath = "C:\vcpkg\vcpkg.exe"
    } elseif (Test-Path "C:\tools\vcpkg\vcpkg.exe") {
        $vcpkgPath = "C:\tools\vcpkg\vcpkg.exe"
    } elseif (Get-Command vcpkg -ErrorAction SilentlyContinue) {
        $vcpkgPath = "vcpkg"
    }
    
    if ($vcpkgPath) {
        Write-Host "[INFO] Installing Boost..." -ForegroundColor Blue
        & $vcpkgPath install boost-system boost-filesystem boost-program-options --triplet x64-windows
        
        Write-Host "[INFO] Installing zlib..." -ForegroundColor Blue
        & $vcpkgPath install zlib --triplet x64-windows
        
        # 设置CMake工具链
        $env:CMAKE_TOOLCHAIN_FILE = "$($vcpkgPath | Split-Path)\scripts\buildsystems\vcpkg.cmake"
        Write-Host "[INFO] Using vcpkg toolchain: $env:CMAKE_TOOLCHAIN_FILE" -ForegroundColor Green
    } else {
        Write-Host "[WARNING] vcpkg not found. Please install dependencies manually:" -ForegroundColor Yellow
        Write-Host "  1. Install vcpkg from https://github.com/Microsoft/vcpkg" -ForegroundColor Yellow
        Write-Host "  2. Run: vcpkg install boost-system boost-filesystem boost-program-options zlib --triplet x64-windows" -ForegroundColor Yellow
    }
}

# 清理构建目录
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "[INFO] Cleaning build directory..." -ForegroundColor Blue
    Remove-Item -Recurse -Force $BuildDir
}

# 创建构建目录
if (-not (Test-Path $BuildDir)) {
    Write-Host "[INFO] Creating build directory..." -ForegroundColor Blue
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# 进入构建目录
Set-Location $BuildDir

# 构建CMake命令
$cmakeArgs = @("..") 
$cmakeArgs += "-DCMAKE_BUILD_TYPE=$BuildType"

if ($Generator) {
    $cmakeArgs += "-G", $Generator
}

if ($env:CMAKE_TOOLCHAIN_FILE) {
    $cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$env:CMAKE_TOOLCHAIN_FILE"
}

# 运行CMake配置
Write-Host "[INFO] Running CMake configuration..." -ForegroundColor Blue
if ($Verbose) {
    Write-Host "Command: cmake $($cmakeArgs -join ' ')" -ForegroundColor Gray
}

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
} catch {
    Write-Host "[ERROR] CMake configuration failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

# 构建项目
Write-Host ""
Write-Host "[INFO] Building project..." -ForegroundColor Blue

$buildArgs = @("--build", ".", "--config", $BuildType)
if ($Verbose) {
    $buildArgs += "--verbose"
}

try {
    & cmake @buildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
} catch {
    Write-Host "[ERROR] Build failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

Set-Location $ProjectRoot

Write-Host ""
Write-Host "[SUCCESS] Build completed successfully!" -ForegroundColor Green

# 查找可执行文件
$exePaths = @(
    "$BuildDir\$BuildType\jvm.exe",
    "$BuildDir\jvm.exe"
)

$exeFound = $false
foreach ($exePath in $exePaths) {
    if (Test-Path $exePath) {
        Write-Host "[INFO] Executable location: $exePath" -ForegroundColor Blue
        Write-Host ""
        Write-Host "[INFO] Testing executable..." -ForegroundColor Blue
        try {
            & $exePath --help
            $exeFound = $true
            break
        } catch {
            Write-Host "[WARNING] Failed to run executable test" -ForegroundColor Yellow
        }
    }
}

if (-not $exeFound) {
    Write-Host "[WARNING] Executable not found in expected locations." -ForegroundColor Yellow
    Write-Host "Check the build directory manually." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Build script completed." -ForegroundColor Green 