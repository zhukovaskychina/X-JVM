//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_FieldRef.h"

u2 ClassFile::Constant_FieldRef::getClassIndex() const {
    return class_index;
}

void ClassFile::Constant_FieldRef::setClassIndex(u2 classIndex) {
    class_index = classIndex;
}

u2 ClassFile::Constant_FieldRef::getNameAndTypeIndex() const {
    return name_and_type_index;
}

void ClassFile::Constant_FieldRef::setNameAndTypeIndex(u2 nameAndTypeIndex) {
    name_and_type_index = nameAndTypeIndex;
}

ClassFile::Constant_FieldRef::~Constant_FieldRef() {

}

ClassFile::Constant_FieldRef::Constant_FieldRef() {

}
