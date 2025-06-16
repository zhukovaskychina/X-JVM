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

### 依赖项
- C++14编译器
- CMake 3.12+
- Boost库 (system, filesystem, program_options)
- zlib库

### 编译步骤
```bash
mkdir build
cd build
cmake ..
make
```

### 运行方式
```bash
./jvm --xjre <JRE路径> --classpath <类路径> --javaclass <主类名.class>
```

### 参数说明
- `--xjre`: JRE主目录路径
- `--classpath`: 用户类路径
- `--javaclass`: 要执行的Java类文件名（必须以.class结尾）
- `--help`: 显示帮助信息

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

## 开发状态

这是一个学习型项目，主要用于理解JVM内部工作原理。目前可以执行简单的Java程序，但还不是一个完整的生产级JVM。

## 许可证

[LICENSE文件](LICENSE)

## 贡献

欢迎提交Issue和Pull Request来改进这个项目。

---

**注意**: 这是一个教育性质的JVM实现，主要用于学习JVM内部机制，不建议用于生产环境。
