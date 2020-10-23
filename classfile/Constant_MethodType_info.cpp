//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_MethodType_info.h"

u2 ClassFile::Constant_MethodTypeInfo::getDescriptorIndex() const {
    return descriptor_index;
}

void ClassFile::Constant_MethodTypeInfo::setDescriptorIndex(u2 descriptorIndex) {
    descriptor_index = descriptorIndex;
}

ClassFile::Constant_MethodTypeInfo::~Constant_MethodTypeInfo() {

}

ClassFile::Constant_MethodTypeInfo::Constant_MethodTypeInfo() {

}
