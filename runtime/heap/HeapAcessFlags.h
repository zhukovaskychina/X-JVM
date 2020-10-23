//
// Created by zhukovasky on 2020/8/19.
//

#ifndef JVM_HEAPACESSFLAGS_H
#define JVM_HEAPACESSFLAGS_H

namespace Runtime{
    enum ClassEnum{
        ACC_PUBLIC =0x0001,
        ACC_PRIVATE=0x0002,
        ACC_PROTECTED=0x0004,
        ACC_STATIC=0x0008,
        ACC_FINAL=0x0010,
        ACC_SUPER=0x0020,
        ACC_SYNCHRONIZED=0x0020,
        ACC_VOLATILE=0x0040,
        ACC_BRIDGE=0x0080,
        ACC_TRANSIENT=0x0080,
        ACC_VARARGS=0x0080,
        ACC_NATIVE=0x0100,
        ACC_INTERFACE=0x0200,
        ACC_ABSTRACT=0X0400,
        ACC_STRICT=0X0800,
        ACC_SYNTHETIC=0X1000,
        ACC_ANNOTATION=0X2000,
        ACC_ENUM=0x4000
    };
}




#endif //JVM_HEAPACESSFLAGS_H
