//
// Created by zhukovasky on 2020/8/29.
//

#ifndef JVM_HEAPCONSTANTUTILS_H
#define JVM_HEAPCONSTANTUTILS_H


#include <string>
#include "../common/Internal.h"
#include "../classfile/ConstantPoolsList.h"

class HeapConstantUtils {
public:
    static std::string getUTF8(u2 index,ClassFile::ConstantPoolsList *classFileConstantsPools);

    static std::string getClassName(u2 index,ClassFile::ConstantPoolsList *constantPools);

    static std::string getDescriptor(u2 index,ClassFile::ConstantPoolsList *constantPools);

    static std::string getFieldOrMethodName(u2 index,ClassFile::ConstantPoolsList *pConstantPools);

};


#endif //JVM_HEAPCONSTANTUTILS_H
