//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_MethodHandle_info.h"

u1 ClassFile::Constant_MethodHandleInfo::getReferenceKind() const {
    return reference_kind;
}

void ClassFile::Constant_MethodHandleInfo::setReferenceKind(u1 referenceKind) {
    reference_kind = referenceKind;
}

u2 ClassFile::Constant_MethodHandleInfo::getReferenceIndex() const {
    return reference_index;
}

void ClassFile::Constant_MethodHandleInfo::setReferenceIndex(u2 referenceIndex) {
    reference_index = referenceIndex;
}

ClassFile::Constant_MethodHandleInfo::~Constant_MethodHandleInfo() {

}

ClassFile::Constant_MethodHandleInfo::Constant_MethodHandleInfo() {

}
