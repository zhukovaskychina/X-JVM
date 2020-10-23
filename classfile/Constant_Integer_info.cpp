//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Integer_info.h"

u4 ClassFile::Constant_IntegerInfo::getBytes() const {
    return bytes;
}

void ClassFile::Constant_IntegerInfo::setBytes(u4 bytes) {
    Constant_IntegerInfo::bytes = bytes;
}

ClassFile::Constant_IntegerInfo::~Constant_IntegerInfo() {

}

ClassFile::Constant_IntegerInfo::Constant_IntegerInfo() {

}
