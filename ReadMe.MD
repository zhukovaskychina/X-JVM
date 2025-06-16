# X-JVM - C++实现的Java虚拟机

**作者**: zhukovasky  
**项目名称**: zhukovasky的Java虚拟机  
**开发语言**: C++14  
**构建系统**: CMake  

## 项目简介

X-JVM是一个用C++14编写的Java虚拟机实现，目标是能够解析、加载和执行Java字节码。该项目实现了JVM的核心组件，包括类文件解析、运行时环境、指令执行引擎和内存管理等功能。

## 项目架构

### 核心模块

#### 1. 类文件解析 (`classfile/`)
- **ClassFile**: Java类文件解析器
- **ConstantPools**: 常量池解析
- **MethodInfo/FieldInfo**: 方法和字段信息解析
- **AttributeInfo**: 属性信息解析（支持多种属性类型）
- 支持完整的Java类文件格式规范

#### 2. 运行时环境 (`runtime/`)
- **ClassLoader**: 类加载器，支持Bootstrap和用户类路径
- **JavaThread**: Java线程实现
- **ThreadPool**: 线程池管理
- **JavaFrame**: 栈帧实现
- **OperandStack**: 操作数栈
- **LocalVariableTables**: 本地变量表
- **Slots**: 变量槽位管理

#### 3. 堆内存管理 (`runtime/heap/`)
- **JavaClass**: 运行时类表示
- **JavaHeap**: 堆内存管理
- **Object**: Java对象模型
- **StringConstantPools**: 字符串常量池
- **MethodDescriptor**: 方法描述符解析
- **ClassMember**: 类成员管理
- **RuntimeConstantsPool**: 运行时常量池

#### 4. 垃圾回收 (`runtime/gc/`)
- **GarbageCollector**: 垃圾回收器框架
- **GCThreadPool**: GC专用线程池
- 支持标记-清除算法基础框架

#### 5. 指令执行 (`instructions/`)
- **InstructionFactory**: 指令工厂
- **常量指令** (`constants/`): NOP, CONST, LDC, BIPUSH, SIPUSH等
- **加载指令** (`load/`): ILOAD, ALOAD等变体
- **存储指令** (`store/`): ISTORE, ASTORE等变体
- **算术指令** (`maths/`): IADD等算术运算
- **栈操作指令** (`stack/`): DUP等栈操作
- **控制流指令** (`control/`): RETURN, GOTO等
- **引用指令** (`reference/`): NEW, GETSTATIC, INVOKESTATIC等
- **比较指令** (`comparisons/`): 各种比较操作

#### 6. 字节码解释器 (`interpret/`)
- **Interpret**: 字节码解释执行引擎
- **ByteCodeReader**: 字节码读取器
- 支持循环执行Java方法

#### 7. 工具类 (`utils/`)
- **FileUtils**: 文件操作工具
- **StringUtils**: 字符串处理工具
- **ZipUtils**: ZIP文件处理（支持JAR文件）

## 编译和运行

### 系统要求
- Windows 10/11 (推荐) 或 Windows 7+
- C++14兼容编译器 (Visual Studio 2017+ 或 MinGW)
- CMake 3.12+
- Git (用于下载依赖)

### 依赖项
- **Boost库**: system, filesystem, program_options
- **zlib库**: 用于ZIP/JAR文件处理

### 🚀 快速开始 (Windows)

#### 方法一：使用自动化脚本 (推荐)

1. **安装依赖** (首次使用):
```cmd
# 运行依赖安装脚本
setup-deps.bat
```

2. **编译项目**:
```cmd
# 使用批处理脚本 (简单)
build.bat

# 或使用PowerShell脚本 (功能更强)
.\build.ps1
```

3. **运行程序**:
```cmd
# 编译完成后
.\build\Release\jvm.exe --help
```

#### 方法二：手动编译

1. **安装依赖**:
```cmd
# 安装vcpkg (如果没有)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖库
vcpkg install boost-system boost-filesystem boost-program-options zlib --triplet x64-windows
vcpkg integrate install
```

2. **编译项目**:
```cmd
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

### 🛠️ 构建选项

#### 批处理脚本选项:
```cmd
build.bat                    # Release构建
build.bat --debug           # Debug构建  
build.bat --clean           # 清理后构建
build.bat --help            # 显示帮助
```

#### PowerShell脚本选项:
```powershell
.\build.ps1                                    # Release构建
.\build.ps1 -BuildType Debug                   # Debug构建
.\build.ps1 -Clean                             # 清理后构建
.\build.ps1 -InstallDeps                       # 自动安装依赖
.\build.ps1 -Generator "MinGW Makefiles"       # 使用MinGW
.\build.ps1 -Verbose                           # 详细输出
.\build.ps1 -Help                              # 显示帮助
```

#### 清理构建文件:
```cmd
clean.bat                    # 清理所有构建文件
```

### 运行方式
```cmd
jvm.exe --xjre <JRE路径> --classpath <类路径> --javaclass <主类名.class>
```

### 参数说明
- `--xjre`: JRE主目录路径
- `--classpath`: 用户类路径
- `--javaclass`: 要执行的Java类文件名（必须以.class结尾）
- `--help`: 显示帮助信息

### 🔧 故障排除

**常见问题:**

1. **CMake找不到Boost库**:
   - 确保使用了vcpkg工具链: `-DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake`
   - 或运行 `setup-deps.bat` 自动配置

2. **编译器错误**:
   - 确保安装了Visual Studio 2017+或MinGW
   - 检查PATH环境变量

3. **PowerShell执行策略错误**:
   ```powershell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

## 支持的JVM特性

### ✅ 已实现功能
1. **类文件解析**: 完整支持Java类文件格式
2. **常量池解析**: 支持所有常量池项类型
3. **类加载**: 支持Bootstrap类路径和用户类路径
4. **基础指令**: 支持常见的JVM指令（约50+条）
5. **栈帧管理**: 完整的栈帧、操作数栈、本地变量表实现
6. **方法调用**: 支持静态方法调用
7. **对象创建**: 基础的对象实例化
8. **内存管理**: 基础堆内存分配
9. **多线程**: 线程池和Java线程抽象

### 🚧 部分实现功能
1. **垃圾回收**: 框架已搭建，具体算法待实现
2. **异常处理**: 异常框架存在，处理逻辑不完整
3. **本地方法**: 接口定义完成，具体实现待补充

## 项目特色

1. **现代C++**: 使用C++14标准，充分利用现代C++特性
2. **模块化设计**: 清晰的模块划分，便于理解和维护
3. **多线程支持**: 内置线程池，支持并发执行
4. **可扩展架构**: 指令工厂模式，便于添加新指令
5. **标准兼容**: 严格按照JVM规范实现
6. **Windows友好**: 提供完整的Windows构建脚本和工具

## 开发状态

这是一个学习型项目，主要用于理解JVM内部工作原理。目前可以执行简单的Java程序，但还不是一个完整的生产级JVM。

**完成度约45%** - 适合学习JVM内部机制的参考项目

## 许可证

[LICENSE文件](LICENSE)

## 贡献

欢迎提交Issue和Pull Request来改进这个项目。

---

**注意**: 这是一个教育性质的JVM实现，主要用于学习JVM内部机制，不建议用于生产环境。
