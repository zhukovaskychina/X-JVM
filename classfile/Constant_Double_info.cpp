//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Double_info.h"

u4 ClassFile::Constant_DoubleInfo::getHighBytes() const {
    return high_bytes;
}

void ClassFile::Constant_DoubleInfo::setHighBytes(u4 highBytes) {
    high_bytes = highBytes;
}

u4 ClassFile::Constant_DoubleInfo::getLowBytes() const {
    return low_bytes;
}

void ClassFile::Constant_DoubleInfo::setLowBytes(u4 lowBytes) {
    low_bytes = lowBytes;
}

ClassFile::Constant_DoubleInfo::~Constant_DoubleInfo() {

}

ClassFile::Constant_DoubleInfo::Constant_DoubleInfo() {

}
