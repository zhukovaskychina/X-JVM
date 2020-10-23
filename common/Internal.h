//
// Created by zhukovasky on 2020/8/4.
//



#include <cstdint>
#include "iostream"



typedef uint32_t u4;
typedef uint16_t u2;
typedef uint8_t u1;
//处理字节码大小端问题
#define getu1(buf) *(u1*)buf

#define getu2(buf) \
    (u2)((((*(u2*)buf) & 0x00FF) << 8) | (((*(u2*)buf) & 0xFF00) >> 8))

#define getu4(buf)                                                           \
    (((*(u4*)buf) & 0x000000FF) << 24) | (((*(u4*)buf) & 0x0000FF00) << 8) | \
        (((*(u4*)buf) & 0x00FF0000) >> 8) | (((*(u4*)buf) & 0xFF000000) >> 24)


////常量池Tag
//enum CONSTANT_POOL_ENUM {
//
//    CONSTANT_Class = 7,
//    CONSTANT_FieldRef = 9,
//    CONSTANT_MethodRef = 10,
//    CONSTANT_InterfaceMethodRef = 11,
//    CONSTANT_String = 8,
//    CONSTANT_Integer = 3,
//    CONSTANT_Float = 4,
//    CONSTANT_Long = 5,
//    CONSTANT_Double = 6,
//    CONSTANT_NameAndType = 12,
//    CONSTANT_Utf8 = 1,
//    CONSTANT_MethodHandle = 15,
//    CONSTANT_MethodType = 16,
//    CONSTANT_InvokeDynamic = 18
//};
//
//enum CLASS_ACCESS_FLAGS_ENUM {
//    ACC_PUBLIC = 0x0001,  //声明为public,可以被包外访问
//    ACC_FINAL = 0x0010,   //声明为final,不允许有子类
//    ACC_SUPER = 0x0020,    //当用到invokespecial指令时，需要特殊处理
//    ACC_INTERFACE = 0x0200,
//    ACC_ABSTRACT = 0x0400,
//    ACC_SYNTHETIC = 0x1000,
//    ACC_ANNOTATION = 0x2000,
//    ACC_ENUM = 0x4000
//};
//
//enum FIELD_ACCESS_FLAGS_ENUM {
//
//    FIELD_ACC_PUBLIC = 0x0001,
//    FIELD_ACC_PRIVATE = 0x0002,
//    FIELD_ACC_PROTECTED = 0x0004,
//    FIELD_ACC_STATIC = 0x0008,
//    FIELD_ACC_FINAL = 0x0010,
//    FIELD_ACC_VOLATILE = 0x0040,
//    FIELD_ACC_TRANSIENT = 0x0080,
//    FIELD_ACC_SYNTHETIC = 0x1000,
//    FIELD_ACC_ENUM = 0x4000
//
//};
//
//enum METHOD_ACCESS_FLAGS_ENUM {
//    METHOD_ACC_PUBLIC = 0x0001,
//    METHOD_ACC_PRIVATE = 0x0002,
//    METHOD_ACC_PROTECTED = 0x0004,
//    METHOD_ACC_STATIC = 0x0008,
//    METHOD_ACC_FINAL = 0x0010,
//    METHOD_ACC_SYNCHRONIZED = 0x0020,
//    METHOD_ACC_BRIDGE = 0X0040,
//    METHOD_ACC_VARAGS = 0X0080,
//    METHOD_ACC_NATIVE = 0X0100,
//    METHOD_ACC_ABSTRACT = 0X0400,
//    METHOD_ACC_STRICT = 0X0800,
//    METHOD_ACC_SYNTHETIC = 0X1000
//};
typedef unsigned char byte;


