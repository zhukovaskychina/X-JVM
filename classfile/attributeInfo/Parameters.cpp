//
// Created by zhukovasky on 2020/8/6.
//

#include "Parameters.h"

ClassFile::Parameters::Parameters() {}

ClassFile::Parameters::~Parameters() {

}

u2 ClassFile::Parameters::getNameIndex() const {
    return name_index;
}

void ClassFile::Parameters::setNameIndex(u2 nameIndex) {
    name_index = nameIndex;
}

u2 ClassFile::Parameters::getAccessFlags() const {
    return access_flags;
}

void ClassFile::Parameters::setAccessFlags(u2 accessFlags) {
    access_flags = accessFlags;
}
