//
// Created by zhukovasky on 2020/4/23.
//

#include "FieldInfo.h"
namespace ClassFile{
    FieldInfo::FieldInfo() {}

    FieldInfo::~FieldInfo() {

    }

    u2 FieldInfo::getAccessFlags() const {
        return access_flags;
    }

    void FieldInfo::setAccessFlags(u2 accessFlags) {
        access_flags = accessFlags;
    }

    u2 FieldInfo::getNameIndex() const {
        return name_index;
    }

    void FieldInfo::setNameIndex(u2 nameIndex) {
        name_index = nameIndex;
    }

    u2 FieldInfo::getDescriptorIndex() const {
        return descriptor_index;
    }

    void FieldInfo::setDescriptorIndex(u2 descriptorIndex) {
        descriptor_index = descriptorIndex;
    }

    u2 FieldInfo::getAttributesCount() const {
        return attributes_count;
    }

    void FieldInfo::setAttributesCount(u2 attributesCount) {
        attributes_count = attributesCount;
    }

    const std::list<AttributesInfo *> &FieldInfo::getAttributesInfoList() const {
        return attributesInfoList;
    }

    void FieldInfo::setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList) {
        FieldInfo::attributesInfoList = attributesInfoList;
    }
}