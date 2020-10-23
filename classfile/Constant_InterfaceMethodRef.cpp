//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_InterfaceMethodRef.h"

u2 ClassFile::Constant_InterfaceMethodRef::getClassIndex() const {
    return class_index;
}

void ClassFile::Constant_InterfaceMethodRef::setClassIndex(u2 classIndex) {
    class_index = classIndex;
}

u2 ClassFile::Constant_InterfaceMethodRef::getNameAndTypeIndex() const {
    return name_and_type_index;
}

void ClassFile::Constant_InterfaceMethodRef::setNameAndTypeIndex(u2 nameAndTypeIndex) {
    name_and_type_index = nameAndTypeIndex;
}

ClassFile::Constant_InterfaceMethodRef::~Constant_InterfaceMethodRef() {

}

ClassFile::Constant_InterfaceMethodRef::Constant_InterfaceMethodRef() {

}
