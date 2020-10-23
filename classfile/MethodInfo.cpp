//
// Created by zhukovasky on 2020/4/23.
//

#include "MethodInfo.h"

namespace ClassFile{


    u2 MethodInfo::getAccessFlags() const {
        return access_flags;
    }

    void MethodInfo::setAccessFlags(u2 accessFlags) {
        access_flags = accessFlags;
    }

    u2 MethodInfo::getNameIndex() const {
        return name_index;
    }

    void MethodInfo::setNameIndex(u2 nameIndex) {
        name_index = nameIndex;
    }

    u2 MethodInfo::getDescriptorIndex() const {
        return descriptor_index;
    }

    void MethodInfo::setDescriptorIndex(u2 descriptorIndex) {
        descriptor_index = descriptorIndex;
    }

    u2 MethodInfo::getAttributesCount() const {
        return attributes_count;
    }

    void MethodInfo::setAttributesCount(u2 attributesCount) {
        attributes_count = attributesCount;
    }

    const std::list<AttributesInfo *> &MethodInfo::getAttributesInfoList() const {
        return attributesInfoList;
    }

    MethodInfo::MethodInfo() {}

    MethodInfo::~MethodInfo() {

    }

    void MethodInfo::setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList) {
        this->attributesInfoList = attributesInfoList;
    }
}