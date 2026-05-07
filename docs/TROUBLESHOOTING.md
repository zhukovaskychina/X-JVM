# X-JVM 故障排除指南

本文档帮助解决X-JVM编译和运行过程中遇到的常见问题。

## 📋 常见编译问题

### 1. CMake找不到Boost库

**错误信息**:
```
Could NOT find Boost (missing: Boost_INCLUDE_DIR system filesystem program_options)
```

**解决方案** (按推荐顺序):

#### Windows:

**方案1: 使用自动化脚本** ⭐推荐
```cmd
scripts\setup-deps.bat
```

**方案2: 使用PowerShell自动安装**
```powershell
.\scripts\build.ps1 -InstallDeps
```

**方案3: 手动安装vcpkg**
```cmd
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
vcpkg install boost-system boost-filesystem boost-program-options zlib --triplet x64-windows
vcpkg integrate install
```

**方案4: 设置BOOST_ROOT环境变量**
```cmd
set BOOST_ROOT=C:\path\to\boost
scripts\build.bat
```

**方案5: MinGW用户使用MSYS2**
```bash
pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-zlib
```

**方案6: MinGW ABI 兼容性问题解决** ⭐推荐
如果你使用MinGW编译器但遇到Boost库ABI不兼容的问题，请使用以下解决方案：

```cmd
# 方案6a: 自动构建MinGW兼容的Boost (推荐)
setup-mingw-boost.bat

# 方案6b: 使用vcpkg的MinGW triplet
setup-vcpkg-mingw.bat

# 方案6c: 手动构建Boost
# 1. 下载Boost源码
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.zip
unzip boost_1_82_0.zip
cd boost_1_82_0

# 2. 配置Boost.Build
bootstrap.bat gcc

# 3. 编译Boost库
b2.exe toolset=gcc --with-system --with-filesystem --with-program_options link=shared,static runtime-link=shared threading=multi variant=release,debug address-model=64 --prefix=C:\boost-mingw install
```

**验证ABI兼容性：**
```cmd
# 检查编译器
g++ --version

# 验证Boost库
g++ -o test_boost test_boost.cpp -IC:\boost-mingw\include -LC:\boost-mingw\lib -lboost_system -lboost_filesystem -lboost_program_options

# 如果成功编译，说明ABI兼容
```

#### Linux:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install libboost-all-dev zlib1g-dev cmake build-essential
```

**CentOS/RHEL:**
```bash
sudo yum install boost-devel zlib-devel cmake gcc-c++ make
```

**Fedora:**
```bash
sudo dnf install boost-devel zlib-devel cmake gcc-c++ make
```

**Arch Linux:**
```bash
sudo pacman -S boost zlib cmake gcc make
```

#### macOS:

```bash
# 安装Homebrew (如果没有)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install boost zlib cmake
```

### 2. CMake版本兼容性警告

**错误信息**:
```
Policy CMP0167 is not set: The FindBoost module is removed
```

**解决方案**:
已在新版CMakeLists.txt中修复，确保使用最新版本的CMakeLists.txt。

### 3. 编译器找不到

**错误信息**:
```
No C++ compiler found
```

**解决方案**:

#### Windows:
- 安装Visual Studio 2017+或Visual Studio Build Tools
- 或安装MinGW: `choco install mingw` (需要Chocolatey)

#### Linux:
```bash
# Ubuntu/Debian
sudo apt install build-essential

# CentOS/RHEL  
sudo yum install gcc-c++ make

# Fedora
sudo dnf install gcc-c++ make
```

#### macOS:
```bash
# 安装Xcode命令行工具
xcode-select --install

# 或使用Homebrew安装GCC
brew install gcc
```

### 4. zlib库找不到

**错误信息**:
```
Neither system zlib nor bundled zlib found
```

**解决方案**:
项目包含内置zlib，这个错误通常不会出现。如果出现:

#### Windows:
```cmd
# 使用vcpkg
vcpkg install zlib --triplet x64-windows
```

#### Linux:
```bash
# Ubuntu/Debian
sudo apt install zlib1g-dev

# CentOS/RHEL
sudo yum install zlib-devel
```

#### macOS:
```bash
brew install zlib
```

## 🔧 运行时问题

### 1. 可执行文件找不到

**现象**: 编译成功但找不到jvm.exe或jvm

**解决方案**:
```bash
# 查找可执行文件
find build -name "jvm*" -type f  # Linux/macOS
dir build /s /b | findstr jvm   # Windows

# 常见位置:
# Windows: build\Release\jvm.exe 或 build\Debug\jvm.exe
# Linux/macOS: build/jvm
```

### 2. 权限问题 (Linux/macOS)

**错误信息**:
```
Permission denied: ./scripts/build.sh
```

**解决方案**:
```bash
chmod +x scripts/build.sh
chmod +x run-example.sh  # 如果存在
```

### 3. PowerShell执行策略错误 (Windows)

**错误信息**:
```
execution of scripts is disabled on this system
```

**解决方案**:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

## 🐛 Java程序运行问题

### 1. 找不到主方法

**错误信息**:
```
java虚拟机找不到主方法
```

**解决方案**:
- 确保Java类包含`public static void main(String[] args)`方法
- 检查类文件路径和classpath设置

### 2. JRE路径错误

**错误信息**:
```
JRE path not found
```

**解决方案**:
```bash
# 查找JRE路径
# Windows
dir "C:\Program Files\Java" /ad

# Linux
find /usr -name "jre*" -type d 2>/dev/null
find /usr -name "java*" -type d 2>/dev/null

# macOS  
ls /Library/Java/JavaVirtualMachines/
```

### 3. 类文件版本不兼容

**现象**: X-JVM可能不支持所有Java版本的类文件

**解决方案**:
- 使用Java 8编译的类文件进行测试
- 检查类文件是否损坏

## 📊 性能和调试

### 1. 启用详细输出

**Windows**:
```cmd
scripts\build.bat         # 基本输出
.\scripts\build.ps1 -Verbose  # 详细输出
```

**Linux/macOS**:
```bash
./scripts/build.sh -v     # 详细输出
```

### 2. Debug构建

**Windows**:
```cmd
scripts\build.bat --debug
.\scripts\build.ps1 -BuildType Debug
```

**Linux/macOS**:
```bash
./scripts/build.sh -t Debug
```

### 3. 清理构建

**Windows**:
```cmd
clean.bat
scripts\build.bat --clean
.\scripts\build.ps1 -Clean
```

**Linux/macOS**:
```bash
./scripts/build.sh -c
# 或手动清理
rm -rf build
```

## 🆘 获取帮助

### 1. 查看帮助信息

```bash
# 构建脚本帮助
scripts\build.bat --help              # Windows
.\scripts\build.ps1 -Help             # Windows PowerShell
./scripts/build.sh -h                 # Linux/macOS

# JVM程序帮助
./jvm --help                  # 运行时帮助
```

### 2. 检查系统信息

**Windows**:
```cmd
cmake --version
gcc --version    # 如果使用MinGW
cl               # 如果使用MSVC
```

**Linux/macOS**:
```bash
cmake --version
gcc --version
clang --version
uname -a
```

### 3. 环境变量检查

**Windows**:
```cmd
echo %BOOST_ROOT%
echo %VCPKG_ROOT%
echo %PATH%
```

**Linux/macOS**:
```bash
echo $BOOST_ROOT
echo $PATH
echo $LD_LIBRARY_PATH  # Linux
echo $DYLD_LIBRARY_PATH  # macOS
```

## 📞 报告问题

如果以上解决方案都无法解决问题，请提供以下信息：

1. 操作系统和版本
2. 编译器和版本
3. CMake版本
4. 完整的错误信息
5. 使用的构建命令
6. 环境变量设置

---

**提示**: 大多数问题都是由于缺少依赖库造成的。推荐首先运行 `scripts\setup-deps.bat` (Windows)；Linux/macOS 请先按 README 安装 Boost/zlib，再执行 `./scripts/build.sh`。 