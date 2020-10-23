//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_NameAndType_info.h"

u2 ClassFile::Constant_NameAndTypeInfo::getNameIndex() const {
    return name_index;
}

void ClassFile::Constant_NameAndTypeInfo::setNameIndex(u2 nameIndex) {
    name_index = nameIndex;
}

u2 ClassFile::Constant_NameAndTypeInfo::getDescriptorIndex() const {
    return descriptor_index;
}

void ClassFile::Constant_NameAndTypeInfo::setDescriptorIndex(u2 descriptorIndex) {
    descriptor_index = descriptorIndex;
}

ClassFile::Constant_NameAndTypeInfo::~Constant_NameAndTypeInfo() {

}

ClassFile::Constant_NameAndTypeInfo::Constant_NameAndTypeInfo() {

}
