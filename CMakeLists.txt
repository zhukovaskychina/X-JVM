cmake_minimum_required(VERSION 3.12)
project(jvm)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost REQUIRED COMPONENTS system filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_ROOT "/usr/include/boost")

include_directories(libs/zlib-1.2.11)
add_subdirectory(libs/zlib-1.2.11)

#set(CMAKE_C_FLAGS -g)
add_executable(jvm main.cpp
        utils/StringUtils.h
        utils/StringUtils.cpp
        utils/FileUtils.h
        utils/FileUtils.cpp
        utils/ZipUtils.h
        utils/ZipUtils.cpp
        libs/unzip/unzip.h
        libs/unzip/unzip.c
        libs/unzip/ioapi.h
        libs/unzip/ioapi.c
        lang/ByteQueue.h
        lang/ByteQueue.cpp
        common/JvmEnums.h
        common/Internal.h
        classfile/Constant_NameAndType_info.cpp
        classfile/Constant_FieldRef.cpp
        classfile/Constant_Float_info.cpp
        classfile/Constant_Class.cpp
        classfile/ConstantPools.cpp
        classfile/AttributesInfo.h
        classfile/Constant_MethodRef.cpp
        classfile/Constant_MethodRef.h
        classfile/Constant_String_info.cpp
        classfile/ClassFile.h
        classfile/Constant_Utf8_info.h
        classfile/Constant_Integer_info.cpp
        classfile/Constant_Utf8_info.cpp
        classfile/Constant_Double_info.cpp
        classfile/Constant_Class.h
        classfile/MethodInfo.cpp
        classfile/MethodInfo.h
        classfile/Constant_MethodHandle_info.cpp
        classfile/Constant_MethodHandle_info.h
        classfile/Constant_InvokeDynamic_info.cpp
        classfile/FieldInfo.cpp
        classfile/Constant_Float_info.h
        classfile/Constant_FieldRef.h
        classfile/AttributesInfo.cpp
        classfile/Constant_Double_info.h
        classfile/Constant_InterfaceMethodRef.cpp
        classfile/Constant_MethodType_info.h
        classfile/ConstantPools.h
        classfile/Constant_Integer_info.h
        classfile/Constant_Long_info.h
        classfile/attributeInfo/LocalVariableTypeTable.h
        classfile/attributeInfo/SyntheticAttribute.h
        classfile/attributeInfo/SignatureAttribute.cpp
        classfile/attributeInfo/InnerClasses.cpp
        classfile/attributeInfo/Code_attribute.cpp
        classfile/attributeInfo/LineNumberTable.cpp
        classfile/attributeInfo/ClassessInner.cpp
        classfile/attributeInfo/LineNumberTable.h
        classfile/attributeInfo/LineNumberTableAttribute.h
        classfile/attributeInfo/LocalVriableTypeTableAttribute.h
        classfile/attributeInfo/SourcFileAttribute.h
        classfile/attributeInfo/ConstantValue.cpp
        classfile/attributeInfo/ConstantValue.h
        classfile/attributeInfo/EnclosingMethdAttribute.h
        classfile/attributeInfo/LineNumberTableAttribute.cpp
        classfile/attributeInfo/LocalVariableTableAttribute.cpp
        classfile/attributeInfo/SourceDebugExtensionAttribute.cpp
        classfile/attributeInfo/SourceDebugExtensionAttribute.h
        classfile/attributeInfo/SignatureAttribute.h
        classfile/attributeInfo/Exceptions_attribute.h
        classfile/attributeInfo/Exception_table.cpp
        classfile/attributeInfo/LocalVariableTable.cpp
        classfile/attributeInfo/SyntheticAttribute.cpp
        classfile/attributeInfo/Exception_table.h
        classfile/attributeInfo/Code_attribute.h
        classfile/attributeInfo/LocalVriableTypeTableAttribute.cpp
        classfile/attributeInfo/Exceptions_attribute.cpp
        classfile/attributeInfo/LocalVariableTable.h
        classfile/attributeInfo/InnerClasses.h
        classfile/attributeInfo/ClassessInner.h
        classfile/attributeInfo/DeprecatedAttribute.cpp
        classfile/attributeInfo/SourcFileAttribute.cpp
        classfile/attributeInfo/ElementValue.h
        classfile/attributeInfo/LocalVariableTableAttribute.h
        classfile/attributeInfo/DeprecatedAttribute.h
        classfile/attributeInfo/LocalVariableTypeTable.cpp
        classfile/attributeInfo/EnclosingMethdAttribute.cpp
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
        classfile/Constant_NameAndType_info.h
        classfile/Constant_InterfaceMethodRef.h
        classfile/Constant_InvokeDynamic_info.h
        classfile/Constant_Long_info.cpp
        classfile/Constant_String_info.h
        classfile/Constant_MethodType_info.cpp
        classfile/FieldInfo.h
        classfile/ClassFile.cpp

        runtime/JavaFrame.cpp
        runtime/JavaFrame.h
        runtime/JavaType.cpp
        runtime/JavaType.h
        runtime/OperandStack.cpp
        runtime/OperandStack.h
        runtime/RuntimeStack.cpp
        runtime/RuntimeStack.h runtime/Object.cpp
        runtime/Object.h runtime/Slots.cpp runtime/Slots.h runtime/ThreadPool.cpp runtime/ThreadPool.h runtime/JavaThread.cpp runtime/JavaThread.h runtime/ClassLoader.cpp runtime/ClassLoader.h
        instructions/base/Instructions.cpp
        instructions/base/Instructions.h
        instructions/base/ByteCodeReader.cpp instructions/base/ByteCodeReader.h instructions/constants/NoOp.cpp instructions/constants/NoOp.h instructions/constants/Const.cpp instructions/constants/Const.h instructions/constants/BiSiPush.cpp instructions/constants/BiSiPush.h instructions/load/Load.cpp instructions/load/Load.h instructions/store/Store.cpp instructions/store/Store.h instructions/stack/StackCommand.cpp instructions/stack/StackCommand.h instructions/maths/Maths.cpp instructions/maths/Maths.h runtime/JavaRuntime.cpp runtime/JavaRuntime.h runtime/heap/Entry.cpp runtime/heap/Entry.h Jvm.cpp Jvm.h runtime/MethodArea.cpp runtime/MethodArea.h classfile/Methods.cpp classfile/Methods.h runtime/heap/JavaHeap.cpp runtime/heap/JavaHeap.h runtime/heap/ClassMember.cpp runtime/heap/ClassMember.h runtime/heap/HeapAcessFlags.h runtime/heap/JavaClass.cpp
        runtime/heap/JavaClass.h runtime/heap/ConstantsPoolObject.cpp runtime/heap/ConstantsPoolObject.h


        instructions/reference/NewRef.cpp instructions/reference/NewRef.h instructions/reference/PutStatic.cpp instructions/reference/PutStatic.h instructions/reference/GetStatic.cpp instructions/reference/GetStatic.h interpret/Interpret.cpp interpret/Interpret.h common/Common.h classfile/ConstantPoolsList.cpp classfile/ConstantPoolsList.h instructions/InstructionFactory.cpp instructions/InstructionFactory.h runtime/HeapConstantUtils.cpp runtime/HeapConstantUtils.h runtime/LocalVariableTables.cpp runtime/LocalVariableTables.h instructions/control/Return.cpp instructions/control/Return.h instructions/control/GoTo.cpp instructions/control/GoTo.h instructions/comparisons/Comparisons.cpp instructions/comparisons/Comparisons.h instructions/reference/InvokeStatic.cpp instructions/reference/InvokeStatic.h runtime/heap/MethodDescriptor.cpp runtime/heap/MethodDescriptor.h instructions/constants/Ldc.cpp instructions/constants/Ldc.h runtime/heap/JavaType.cpp runtime/heap/JavaType.h runtime/heap/StringConstantPools.cpp runtime/heap/StringConstantPools.h instructions/reference/Array.cpp instructions/reference/Array.h instructions/load/Aload.cpp instructions/load/Aload.h instructions/store/AStore.cpp instructions/store/AStore.h runtime/heap/StrongRoots.cpp runtime/heap/StrongRoots.h runtime/gc/GarbageCollector.cpp runtime/gc/GarbageCollector.h runtime/gc/GarbageStatus.cpp runtime/gc/GarbageStatus.h)
link_directories(... ${Boost_LIBRARY_DIRS})
target_link_libraries(jvm pthread zlib ${Boost_LIBRARIES})