//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Float_info.h"

u4 ClassFile::Constant_FloatInfo::getBytes() const {
    return bytes;
}

void ClassFile::Constant_FloatInfo::setBytes(u4 bytes) {
    Constant_FloatInfo::bytes = bytes;
}

ClassFile::Constant_FloatInfo::~Constant_FloatInfo() {

}

ClassFile::Constant_FloatInfo::Constant_FloatInfo() {

}
