//
// Created by zhukovasky on 2020/4/26.
//

#include "ClassessInner.h"

ClassessInner::ClassessInner() {}

ClassessInner::~ClassessInner() {

}

u2 ClassessInner::getInnerClassInfoIndex() const {
    return inner_class_info_index;
}

void ClassessInner::setInnerClassInfoIndex(u2 innerClassInfoIndex) {
    inner_class_info_index = innerClassInfoIndex;
}

u2 ClassessInner::getPuterClassInfoIndex() const {
    return puter_class_info_index;
}

void ClassessInner::setPuterClassInfoIndex(u2 puterClassInfoIndex) {
    puter_class_info_index = puterClassInfoIndex;
}

u2 ClassessInner::getInnerNameIndex() const {
    return inner_name_index;
}

void ClassessInner::setInnerNameIndex(u2 innerNameIndex) {
    inner_name_index = innerNameIndex;
}

u2 ClassessInner::getInnerClassAccessFlags() const {
    return inner_class_access_flags;
}

void ClassessInner::setInnerClassAccessFlags(u2 innerClassAccessFlags) {
    inner_class_access_flags = innerClassAccessFlags;
}
