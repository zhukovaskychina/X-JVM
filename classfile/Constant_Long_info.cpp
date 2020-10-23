//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Long_info.h"

u4 ClassFile::Constant_LongInfo::getHighBytes() const {
    return high_bytes;
}

void ClassFile::Constant_LongInfo::setHighBytes(u4 highBytes) {
    high_bytes = highBytes;
}

u4 ClassFile::Constant_LongInfo::getLowBytes() const {
    return low_bytes;
}

void ClassFile::Constant_LongInfo::setLowBytes(u4 lowBytes) {
    low_bytes = lowBytes;
}

ClassFile::Constant_LongInfo::~Constant_LongInfo() {

}

ClassFile::Constant_LongInfo::Constant_LongInfo() {

}
