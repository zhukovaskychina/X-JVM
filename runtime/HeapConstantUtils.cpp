//
// Created by zhukovasky on 2020/8/29.
//

#include "HeapConstantUtils.h"
#include "../classfile/Constant_Utf8_info.h"
#include "../classfile/Constant_Class.h"
#include "../classfile/Constant_NameAndType_info.h"

std::string HeapConstantUtils::getUTF8(u2 index, ClassFile::ConstantPoolsList *classFileConstantsPools) {
    ClassFile::Constant_Utf8Info *constantUtf8Info= reinterpret_cast<ClassFile::Constant_Utf8Info*>(classFileConstantsPools->getIndex(index));

    std::string result=constantUtf8Info->toString();
    return result;
}

std::string HeapConstantUtils::getClassName(u2 index, ClassFile::ConstantPoolsList *constantPools) {
    ClassFile::Constant_Class *constantClass= reinterpret_cast<ClassFile::Constant_Class*>(constantPools->getIndex(index));
    u2 nameIndex=constantClass->getNameIndex();
    return getUTF8(nameIndex,constantPools);
}

std::string HeapConstantUtils::getDescriptor(u2 index, ClassFile::ConstantPoolsList *constantPools) {
    ClassFile::Constant_NameAndTypeInfo *constantNameAndTypeInfo= reinterpret_cast<ClassFile::Constant_NameAndTypeInfo*>(constantPools->getIndex(index));
    u2 descriptorIndex=constantNameAndTypeInfo->getDescriptorIndex();
    return getUTF8(descriptorIndex,constantPools);
}

std::string HeapConstantUtils::getFieldOrMethodName(u2 index, ClassFile::ConstantPoolsList *pConstantPools) {
    ClassFile::Constant_NameAndTypeInfo *constantNameAndTypeInfo= reinterpret_cast<ClassFile::Constant_NameAndTypeInfo*>(pConstantPools->getIndex(index));
    u2 nameIndex=constantNameAndTypeInfo->getNameIndex();
    return getUTF8(nameIndex,pConstantPools);;
}

