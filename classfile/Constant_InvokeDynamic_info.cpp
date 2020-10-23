//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_InvokeDynamic_info.h"

u2 ClassFile::Constant_InvokeDynamicInfo::getBootstrapMethodAttrIndex() const {
    return bootstrap_method_attr_index;
}

void ClassFile::Constant_InvokeDynamicInfo::setBootstrapMethodAttrIndex(u2 bootstrapMethodAttrIndex) {
    bootstrap_method_attr_index = bootstrapMethodAttrIndex;
}

u2 ClassFile::Constant_InvokeDynamicInfo::getNameAndTypeIndex() const {
    return name_and_type_index;
}

void ClassFile::Constant_InvokeDynamicInfo::setNameAndTypeIndex(u2 nameAndTypeIndex) {
    name_and_type_index = nameAndTypeIndex;
}

ClassFile::Constant_InvokeDynamicInfo::~Constant_InvokeDynamicInfo() {

}

ClassFile::Constant_InvokeDynamicInfo::Constant_InvokeDynamicInfo() {

}
