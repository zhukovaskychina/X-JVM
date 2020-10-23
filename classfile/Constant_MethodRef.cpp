//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_MethodRef.h"

u2 ClassFile::Constant_MethodRef::getClassIndex() const {
    return class_index;
}

void ClassFile::Constant_MethodRef::setClassIndex(u2 classIndex) {
    class_index = classIndex;
}

u2 ClassFile::Constant_MethodRef::getNameAndTypeIndex() const {
    return name_and_type_index;
}

void ClassFile::Constant_MethodRef::setNameAndTypeIndex(u2 nameAndTypeIndex) {
    name_and_type_index = nameAndTypeIndex;
}

ClassFile::Constant_MethodRef::~Constant_MethodRef() {

}

ClassFile::Constant_MethodRef::Constant_MethodRef() {

}
