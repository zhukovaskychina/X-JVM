//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Utf8_info.h"

u2 ClassFile::Constant_Utf8Info::getLength() const {
    return length;
}

void ClassFile::Constant_Utf8Info::setLength(u2 length) {
    Constant_Utf8Info::length = length;
}

u1 *ClassFile::Constant_Utf8Info::getBytes() const {
    return bytes;
}

void ClassFile::Constant_Utf8Info::setBytes(u1 *bytes) {
    Constant_Utf8Info::bytes = bytes;
}

ClassFile::Constant_Utf8Info::~Constant_Utf8Info() {

}

ClassFile::Constant_Utf8Info::Constant_Utf8Info() {

}

std::string ClassFile::Constant_Utf8Info::toString() {
    return std::string(reinterpret_cast<const char *>(this->getBytes()));
}
