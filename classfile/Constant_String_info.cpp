//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_String_info.h"

u2 ClassFile::Constant_StringInfo::getStringIndex() const {
    return string_index;
}

void ClassFile::Constant_StringInfo::setStringIndex(u2 stringIndex) {
    string_index = stringIndex;
}

ClassFile::Constant_StringInfo::~Constant_StringInfo() {

}

ClassFile::Constant_StringInfo::Constant_StringInfo() {

}
