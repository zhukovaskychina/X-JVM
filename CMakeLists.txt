# X-JVM Cross-Platform CMakeLists.txt
# Supports: Linux, macOS, Windows
cmake_minimum_required(VERSION 3.12)

# 设置CMake策略以支持新版本
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)  # 使用新的FindBoost模块行为
endif()

if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)  # 使用PackageName_ROOT变量
endif()

if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)  # 使用新的PACKAGENAME_ROOT变量格式
endif()

# 项目配置
project(jvm VERSION 1.0.0 LANGUAGES CXX C)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型默认值
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)  # CLion默认使用Debug
endif()

# 打印构建信息
message(STATUS "Building X-JVM on ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# 平台检测
if(WIN32)
    message(STATUS "Configuring for Windows")
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    message(STATUS "Configuring for macOS")
    set(PLATFORM_MACOS TRUE)
elseif(UNIX)
    message(STATUS "Configuring for Linux")
    set(PLATFORM_LINUX TRUE)
endif()

# 编译器特定配置
if(MSVC)
    # MSVC编译器设置
    message(STATUS "Configuring for MSVC")
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        _MSVC_VER  # 用于条件编译
    )
    # 禁用某些警告
    add_compile_options(/W3 /wd4267 /wd4244 /wd4996 /wd4834 /wd4018 /wd4101 /wd4700)
    # 添加UTF-8编码支持
    add_compile_options(/utf-8)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC编译器设置
    message(STATUS "Configuring for GCC")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang编译器设置
    message(STATUS "Configuring for Clang")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
endif()

# vcpkg集成 - 专为CLion优化
if(WIN32)
    # 检查vcpkg环境
    if(DEFINED ENV{VCPKG_ROOT})
        message(STATUS "VCPKG_ROOT found: $ENV{VCPKG_ROOT}")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    elseif(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Using C:/vcpkg")
        set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    elseif(EXISTS "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Using C:/tools/vcpkg")
        set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
    
    # 设置vcpkg目标架构 - 智能检测编译器类型
    if(NOT DEFINED VCPKG_TARGET_TRIPLET)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR 
           CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
           CMAKE_CXX_COMPILER MATCHES ".*mingw.*" OR
           CMAKE_C_COMPILER MATCHES ".*mingw.*")
            # 使用MinGW
            set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
            message(STATUS "Detected MinGW/GCC compiler, using x64-mingw-dynamic triplet")
        else()
            # 使用MSVC
            set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
            message(STATUS "Detected MSVC compiler, using x64-windows triplet")
        endif()
    endif()
    
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
endif()

# 依赖库查找配置
set(Boost_USE_STATIC_LIBS OFF)  # vcpkg默认使用动态库
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# MinGW 特殊配置
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    message(STATUS "Configuring for MinGW on Windows")
    
    # 优先查找 MinGW 编译的 Boost
    set(MINGW_BOOST_SEARCH_PATHS
        "C:/boost-mingw"
        "C:/msys64/mingw64"
        "C:/msys64/usr/local"
        "${CMAKE_PREFIX_PATH}"
        "$ENV{BOOST_ROOT}"
    )
    
    foreach(path ${MINGW_BOOST_SEARCH_PATHS})
        if(EXISTS "${path}")
            list(APPEND CMAKE_PREFIX_PATH "${path}")
            message(STATUS "Added MinGW Boost search path: ${path}")
        endif()
    endforeach()
    
    # 设置 MinGW 链接器标志
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Windows下使用vcpkg时的特殊设置
if(WIN32 AND CMAKE_TOOLCHAIN_FILE)
    # vcpkg会自动处理这些设置
    message(STATUS "Using vcpkg toolchain for dependency management")
else()
    # 手动设置Boost路径 (fallback)
    if(WIN32 AND NOT DEFINED BOOST_ROOT AND NOT DEFINED ENV{BOOST_ROOT})
        set(BOOST_SEARCH_PATHS
            "C:/boost-mingw"
            "C:/boost"
            "C:/boost_1_82_0"
            "C:/boost_1_81_0"
            "C:/boost_1_80_0"
            "C:/local/boost"
            "C:/vcpkg/installed/x64-windows"
            "C:/tools/vcpkg/installed/x64-windows"
            "C:/vcpkg/installed/x64-mingw-dynamic"
            "C:/tools/vcpkg/installed/x64-mingw-dynamic"
        )
        
        foreach(path ${BOOST_SEARCH_PATHS})
            if(EXISTS "${path}")
                set(BOOST_ROOT "${path}")
                message(STATUS "Auto-detected Boost root: ${BOOST_ROOT}")
                break()
            endif()
        endforeach()
    endif()
endif()

# 查找依赖库 - 优先使用CONFIG模式 (vcpkg友好)
find_package(Boost QUIET CONFIG COMPONENTS system filesystem program_options)

if(Boost_FOUND)
    message(STATUS "Found Boost using CONFIG mode")
    if(TARGET Boost::boost)
        message(STATUS "Found Boost ${Boost_VERSION} (modern targets)")
        set(BOOST_TARGETS Boost::system Boost::filesystem Boost::program_options)
    else()
        message(STATUS "Found Boost (legacy mode)")
        set(BOOST_TARGETS ${Boost_LIBRARIES})
    endif()
else()
    # 回退到MODULE模式查找
    message(STATUS "Boost CONFIG mode failed, trying MODULE mode...")
    
    # 临时禁用CMP0167警告
    if(POLICY CMP0167)
        cmake_policy(PUSH)
        cmake_policy(SET CMP0167 OLD)
    endif()
    
    find_package(Boost COMPONENTS system filesystem program_options)
    
    if(POLICY CMP0167)
        cmake_policy(POP)
    endif()
    
    if(Boost_FOUND)
        message(STATUS "Found Boost ${Boost_VERSION} using MODULE mode")
        message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
        message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
        set(BOOST_TARGETS ${Boost_LIBRARIES})
    else()
        message(FATAL_ERROR "
=== Boost 未找到! ===
请确保已安装Boost库。

Windows用户请运行:
  .\\setup-deps.bat

或手动使用vcpkg:
  vcpkg install boost-system boost-filesystem boost-program-options zlib --triplet x64-windows

Linux用户:
  sudo apt install libboost-all-dev  # Ubuntu/Debian
  sudo yum install boost-devel       # CentOS/RHEL

macOS用户:
  brew install boost
================")
    endif()
endif()

# 查找zlib
find_package(ZLIB)
if(ZLIB_FOUND)
    message(STATUS "Found system zlib: ${ZLIB_VERSION_STRING}")
    set(ZLIB_TARGET ZLIB::ZLIB)
    set(USE_BUNDLED_ZLIB FALSE)
else()
    message(STATUS "System zlib not found")
    # 检查vcpkg是否有zlib
    find_package(zlib QUIET CONFIG)
    if(zlib_FOUND)
        message(STATUS "Found zlib via vcpkg")
        set(ZLIB_TARGET zlib::zlib)
        set(USE_BUNDLED_ZLIB FALSE)
    else()
        message(STATUS "Neither system nor vcpkg zlib found, using bundled zlib")
        # 使用内置zlib
        set(USE_BUNDLED_ZLIB TRUE)
        
        # 添加内置zlib子目录
        add_subdirectory(libs/zlib-1.2.11)
        
        # 设置zlib目标
        set(ZLIB_TARGET zlib)
        
        # 手动设置zlib变量（兼容旧代码）
        set(ZLIB_FOUND TRUE)
        set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib-1.2.11 ${CMAKE_CURRENT_BINARY_DIR}/libs/zlib-1.2.11)
        set(ZLIB_LIBRARIES zlib)
        set(ZLIB_VERSION_STRING "1.2.11")
        
        message(STATUS "Using bundled zlib: ${ZLIB_VERSION_STRING}")
    endif()
endif()

# 线程库查找
find_package(Threads REQUIRED)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 处理Boost包含目录
if(Boost_INCLUDE_DIRS)
    include_directories(${Boost_INCLUDE_DIRS})
elseif(TARGET Boost::boost)
    # 现代CMake会自动处理包含目录
else()
    message(WARNING "Boost include directories not found")
endif()

# 如果使用内置zlib，添加其包含目录
if(USE_BUNDLED_ZLIB)
    include_directories(${ZLIB_INCLUDE_DIRS})
endif()

# 源文件列表（分类组织）
set(UTILS_SOURCES
    utils/StringUtils.h
    utils/StringUtils.cpp
    utils/FileUtils.h
    utils/FileUtils.cpp
    utils/ZipUtils.h
    utils/ZipUtils.cpp
)

set(LIBS_SOURCES
    libs/unzip/unzip.h
    libs/unzip/unzip.c
    libs/unzip/ioapi.h
    libs/unzip/ioapi.c
)

set(LANG_SOURCES
    lang/ByteQueue.h
    lang/ByteQueue.cpp
)

set(COMMON_SOURCES
    common/JvmEnums.h
    common/Internal.h
    common/Common.h
)

set(CLASSFILE_SOURCES
    classfile/Constant_NameAndType_info.cpp
    classfile/Constant_NameAndType_info.h
    classfile/Constant_FieldRef.cpp
    classfile/Constant_FieldRef.h
    classfile/Constant_Float_info.cpp
    classfile/Constant_Float_info.h
    classfile/Constant_Class.cpp
    classfile/Constant_Class.h
    classfile/ConstantPools.cpp
    classfile/ConstantPools.h
    classfile/AttributesInfo.h
    classfile/AttributesInfo.cpp
    classfile/Constant_MethodRef.cpp
    classfile/Constant_MethodRef.h
    classfile/Constant_String_info.cpp
    classfile/Constant_String_info.h
    classfile/ClassFile.h
    classfile/ClassFile.cpp
    classfile/Constant_Utf8_info.h
    classfile/Constant_Utf8_info.cpp
    classfile/Constant_Integer_info.cpp
    classfile/Constant_Integer_info.h
    classfile/Constant_Double_info.cpp
    classfile/Constant_Double_info.h
    classfile/MethodInfo.cpp
    classfile/MethodInfo.h
    classfile/Constant_MethodHandle_info.cpp
    classfile/Constant_MethodHandle_info.h
    classfile/Constant_InvokeDynamic_info.cpp
    classfile/Constant_InvokeDynamic_info.h
    classfile/FieldInfo.cpp
    classfile/FieldInfo.h
    classfile/Constant_InterfaceMethodRef.cpp
    classfile/Constant_InterfaceMethodRef.h
    classfile/Constant_MethodType_info.h
    classfile/Constant_MethodType_info.cpp
    classfile/Constant_Long_info.h
    classfile/Constant_Long_info.cpp
    classfile/Methods.cpp
    classfile/Methods.h
    classfile/ConstantPoolsList.cpp
    classfile/ConstantPoolsList.h
)

set(CLASSFILE_ATTR_SOURCES
    classfile/attributeInfo/LocalVariableTypeTable.h
    classfile/attributeInfo/LocalVariableTypeTable.cpp
    classfile/attributeInfo/SyntheticAttribute.h
    classfile/attributeInfo/SyntheticAttribute.cpp
    classfile/attributeInfo/SignatureAttribute.cpp
    classfile/attributeInfo/SignatureAttribute.h
    classfile/attributeInfo/InnerClasses.cpp
    classfile/attributeInfo/InnerClasses.h
    classfile/attributeInfo/Code_attribute.cpp
    classfile/attributeInfo/Code_attribute.h
    classfile/attributeInfo/LineNumberTable.cpp
    classfile/attributeInfo/LineNumberTable.h
    classfile/attributeInfo/ClassessInner.cpp
    classfile/attributeInfo/ClassessInner.h
    classfile/attributeInfo/LineNumberTableAttribute.h
    classfile/attributeInfo/LineNumberTableAttribute.cpp
    classfile/attributeInfo/LocalVriableTypeTableAttribute.h
    classfile/attributeInfo/LocalVriableTypeTableAttribute.cpp
    classfile/attributeInfo/SourcFileAttribute.h
    classfile/attributeInfo/SourcFileAttribute.cpp
    classfile/attributeInfo/ConstantValue.cpp
    classfile/attributeInfo/ConstantValue.h
    classfile/attributeInfo/EnclosingMethdAttribute.h
    classfile/attributeInfo/EnclosingMethdAttribute.cpp
    classfile/attributeInfo/LocalVariableTableAttribute.cpp
    classfile/attributeInfo/LocalVariableTableAttribute.h
    classfile/attributeInfo/SourceDebugExtensionAttribute.cpp
    classfile/attributeInfo/SourceDebugExtensionAttribute.h
    classfile/attributeInfo/Exceptions_attribute.h
    classfile/attributeInfo/Exceptions_attribute.cpp
    classfile/attributeInfo/Exception_table.cpp
    classfile/attributeInfo/Exception_table.h
    classfile/attributeInfo/LocalVariableTable.cpp
    classfile/attributeInfo/LocalVariableTable.h
    classfile/attributeInfo/DeprecatedAttribute.cpp
    classfile/attributeInfo/DeprecatedAttribute.h
    classfile/attributeInfo/ElementValue.h
    classfile/attributeInfo/ElementValue.cpp
    classfile/attributeInfo/ElementValuePairs.h
    classfile/attributeInfo/ElementValuePairs.cpp
    classfile/attributeInfo/Annotation.h
    classfile/attributeInfo/Annotation.cpp
    classfile/attributeInfo/RuntimeVisibleAnnotations.h
    classfile/attributeInfo/RuntimeVisibleAnnotations.cpp
    classfile/attributeInfo/RuntimeInvisibleAnnotations.h
    classfile/attributeInfo/RuntimeInvisibleAnnotations.cpp
    classfile/attributeInfo/RuntimeVisibleParameterAnnotationAttributes.h
    classfile/attributeInfo/RuntimeVisibleParameterAnnotationAttributes.cpp
    classfile/attributeInfo/RuntimeInvisibleParameterAnnotationAttributes.h
    classfile/attributeInfo/RuntimeInvisibleParameterAnnotationAttributes.cpp
    classfile/attributeInfo/BootstrapMethodAttribute.cpp
    classfile/attributeInfo/BootstrapMethodAttribute.h
    classfile/attributeInfo/BootstrapMethod.h
    classfile/attributeInfo/BootstrapMethod.cpp
    classfile/attributeInfo/Parameters.h
    classfile/attributeInfo/Parameters.cpp
    classfile/attributeInfo/MethodParameterAttributes.h
    classfile/attributeInfo/MethodParameterAttributes.cpp
    classfile/attributeInfo/ParameterAnnotations.h
    classfile/attributeInfo/ParameterAnnotations.cpp
    classfile/attributeInfo/AnnotationDefaultAttribute.h
    classfile/attributeInfo/AnnotationDefaultAttribute.cpp
    classfile/attributeInfo/StackMapTableAttributes.h
    classfile/attributeInfo/StackMapTableAttributes.cpp
    classfile/attributeInfo/StackMapFrame.h
    classfile/attributeInfo/StackMapFrame.cpp
)

set(RUNTIME_SOURCES
    runtime/JavaFrame.cpp
    runtime/JavaFrame.h
    runtime/JavaType.cpp
    runtime/JavaType.h
    runtime/OperandStack.cpp
    runtime/OperandStack.h
    runtime/RuntimeStack.cpp
    runtime/RuntimeStack.h
    runtime/Object.cpp
    runtime/Object.h
    runtime/Slots.cpp
    runtime/Slots.h
    runtime/ThreadPool.cpp
    runtime/ThreadPool.h
    runtime/JavaThread.cpp
    runtime/JavaThread.h
    runtime/ClassLoader.cpp
    runtime/ClassLoader.h
    runtime/JavaRuntime.cpp
    runtime/JavaRuntime.h
    runtime/MethodArea.cpp
    runtime/MethodArea.h
    runtime/HeapConstantUtils.cpp
    runtime/HeapConstantUtils.h
    runtime/LocalVariableTables.cpp
    runtime/LocalVariableTables.h
)

set(RUNTIME_HEAP_SOURCES
    runtime/heap/Entry.cpp
    runtime/heap/Entry.h
    runtime/heap/JavaHeap.cpp
    runtime/heap/JavaHeap.h
    runtime/heap/ClassMember.cpp
    runtime/heap/ClassMember.h
    runtime/heap/HeapAcessFlags.h
    runtime/heap/JavaClass.cpp
    runtime/heap/JavaClass.h
    runtime/heap/ConstantsPoolObject.cpp
    runtime/heap/ConstantsPoolObject.h
    runtime/heap/MethodDescriptor.cpp
    runtime/heap/MethodDescriptor.h
    runtime/heap/JavaType.cpp
    runtime/heap/JavaType.h
    runtime/heap/StringConstantPools.cpp
    runtime/heap/StringConstantPools.h
    runtime/heap/StrongRoots.cpp
    runtime/heap/StrongRoots.h
)

set(RUNTIME_GC_SOURCES
    runtime/gc/GarbageCollector.cpp
    runtime/gc/GarbageCollector.h
    runtime/gc/GarbageStatus.cpp
    runtime/gc/GarbageStatus.h
)

set(INSTRUCTIONS_SOURCES
    instructions/base/Instructions.cpp
    instructions/base/Instructions.h
    instructions/base/ByteCodeReader.cpp
    instructions/base/ByteCodeReader.h
    instructions/constants/NoOp.cpp
    instructions/constants/NoOp.h
    instructions/constants/Const.cpp
    instructions/constants/Const.h
    instructions/constants/BiSiPush.cpp
    instructions/constants/BiSiPush.h
    instructions/constants/Ldc.cpp
    instructions/constants/Ldc.h
    instructions/load/Load.cpp
    instructions/load/Load.h
    instructions/load/Aload.cpp
    instructions/load/Aload.h
    instructions/store/Store.cpp
    instructions/store/Store.h
    instructions/store/AStore.cpp
    instructions/store/AStore.h
    instructions/stack/StackCommand.cpp
    instructions/stack/StackCommand.h
    instructions/maths/Maths.cpp
    instructions/maths/Maths.h
    instructions/control/Return.cpp
    instructions/control/Return.h
    instructions/control/GoTo.cpp
    instructions/control/GoTo.h
    instructions/comparisons/Comparisons.cpp
    instructions/comparisons/Comparisons.h
    instructions/reference/NewRef.cpp
    instructions/reference/NewRef.h
    instructions/reference/PutStatic.cpp
    instructions/reference/PutStatic.h
    instructions/reference/GetStatic.cpp
    instructions/reference/GetStatic.h
    instructions/reference/InvokeStatic.cpp
    instructions/reference/InvokeStatic.h
    instructions/reference/Array.cpp
    instructions/reference/Array.h
    instructions/InstructionFactory.cpp
    instructions/InstructionFactory.h
)

set(INTERPRET_SOURCES
    interpret/Interpret.cpp
    interpret/Interpret.h
)

set(JVM_SOURCES
    Jvm.cpp
    Jvm.h
)

# 主程序
set(MAIN_SOURCE main.cpp)

# 合并所有源文件
set(ALL_SOURCES
    ${MAIN_SOURCE}
    ${UTILS_SOURCES}
    ${LIBS_SOURCES}
    ${LANG_SOURCES}
    ${COMMON_SOURCES}
    ${CLASSFILE_SOURCES}
    ${CLASSFILE_ATTR_SOURCES}
    ${RUNTIME_SOURCES}
    ${RUNTIME_HEAP_SOURCES}
    ${RUNTIME_GC_SOURCES}
    ${INSTRUCTIONS_SOURCES}
    ${INTERPRET_SOURCES}
    ${JVM_SOURCES}
)

# 创建可执行文件
add_executable(jvm ${ALL_SOURCES})

# 设置目标属性
set_target_properties(jvm PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 平台特定的链接库
set(PLATFORM_LIBS "")

# Windows特定设置
if(WIN32)
    # Windows需要的额外库
    list(APPEND PLATFORM_LIBS ws2_32 wsock32)
    
    # 设置Windows子系统（控制台应用）
    set_target_properties(jvm PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# macOS特定设置
if(APPLE)
    # macOS特定的框架或库
    find_library(CORE_FOUNDATION CoreFoundation)
    if(CORE_FOUNDATION)
        list(APPEND PLATFORM_LIBS ${CORE_FOUNDATION})
    endif()
endif()

# Linux特定设置
if(PLATFORM_LINUX)
    # Linux可能需要的额外库
    list(APPEND PLATFORM_LIBS dl)
endif()

# 链接库 - 修复Boost链接问题
target_link_libraries(jvm
    ${ZLIB_TARGET}
    Threads::Threads
    ${PLATFORM_LIBS}
)

# 添加Boost库链接 - 根据发现方式选择合适的目标
if(TARGET Boost::system AND TARGET Boost::filesystem AND TARGET Boost::program_options)
    # 使用现代CMake目标 (vcpkg/CONFIG模式)
    target_link_libraries(jvm
        Boost::system
        Boost::filesystem  
        Boost::program_options
    )
    message(STATUS "Linking Boost using modern targets")
elseif(Boost_LIBRARIES)
    # 使用传统变量 (MODULE模式)
    target_link_libraries(jvm ${Boost_LIBRARIES})
    message(STATUS "Linking Boost using legacy variables: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "No valid Boost linking method found!")
endif()

# Windows下使用vcpkg时的特殊设置
if(WIN32 AND CMAKE_TOOLCHAIN_FILE)
    # 添加必要的编译定义以避免ABI问题
    target_compile_definitions(jvm PRIVATE
        BOOST_ALL_DYN_LINK=1
        BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    )
endif()

# 设置链接目录 (仅当使用传统Boost变量时)
if(Boost_LIBRARY_DIRS AND NOT TARGET Boost::boost)
    target_link_directories(jvm PRIVATE ${Boost_LIBRARY_DIRS})
endif()

# 编译定义
target_compile_definitions(jvm PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# IDE支持：为源文件分组
if(MSVC OR XCODE)
    source_group("Source Files\\Utils" FILES ${UTILS_SOURCES})
    source_group("Source Files\\Libs" FILES ${LIBS_SOURCES})
    source_group("Source Files\\Lang" FILES ${LANG_SOURCES})
    source_group("Source Files\\Common" FILES ${COMMON_SOURCES})
    source_group("Source Files\\ClassFile" FILES ${CLASSFILE_SOURCES})
    source_group("Source Files\\ClassFile\\Attributes" FILES ${CLASSFILE_ATTR_SOURCES})
    source_group("Source Files\\Runtime" FILES ${RUNTIME_SOURCES})
    source_group("Source Files\\Runtime\\Heap" FILES ${RUNTIME_HEAP_SOURCES})
    source_group("Source Files\\Runtime\\GC" FILES ${RUNTIME_GC_SOURCES})
    source_group("Source Files\\Instructions" FILES ${INSTRUCTIONS_SOURCES})
    source_group("Source Files\\Interpret" FILES ${INTERPRET_SOURCES})
    source_group("Source Files\\JVM" FILES ${JVM_SOURCES})
    source_group("Source Files" FILES ${MAIN_SOURCE})
endif()

# 安装配置
install(TARGETS jvm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 打包配置（可选）
include(CPack)
set(CPACK_PACKAGE_NAME "X-JVM")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "X-JVM - A Java Virtual Machine implementation in C++")
set(CPACK_PACKAGE_VENDOR "zhukovasky")

# 构建信息摘要
message(STATUS "=== X-JVM Build Configuration ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(Boost_FOUND)
    if(Boost_VERSION)
        message(STATUS "Boost Version: ${Boost_VERSION}")
    elseif(Boost_VERSION_STRING)
        message(STATUS "Boost Version: ${Boost_VERSION_STRING}")
    endif()
endif()
if(ZLIB_FOUND)
    message(STATUS "ZLIB: System (${ZLIB_VERSION_STRING})")
else()
    message(STATUS "ZLIB: Bundled")
endif()
message(STATUS "===================================")