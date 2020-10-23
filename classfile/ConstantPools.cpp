//
// Created by zhukovasky on 2020/4/23.
//

#include "ConstantPools.h"

u1 ClassFile::ConstantPools::getTag() const {
    return tag;
}

void ClassFile::ConstantPools::setTag(u1 tag) {
    ConstantPools::tag = tag;
}

ClassFile::ConstantPools::~ConstantPools() {

}

ClassFile::ConstantPools::ConstantPools() {

}
