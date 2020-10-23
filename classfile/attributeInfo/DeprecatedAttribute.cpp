//
// Created by zhukovasky on 2020/4/26.
//

#include "DeprecatedAttribute.h"
namespace ClassFile{
    DeprecatedAttribute::DeprecatedAttribute() {}

    DeprecatedAttribute::~DeprecatedAttribute() {

    }

    void DeprecatedAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
        AttributesInfo::setAttributeNameIndex(attributeNameIndex);
    }

    void DeprecatedAttribute::setAttributeLength(u4 attributeLength) {
        AttributesInfo::setAttributeLength(attributeLength);
    }

    u2 DeprecatedAttribute::getAttributeNameIndex() const {
        return AttributesInfo::getAttributeNameIndex();
    }

    u4 DeprecatedAttribute::getAttributeLength() const {
        return AttributesInfo::getAttributeLength();
    }

    const std::string DeprecatedAttribute::getTags() const {
        return AttributesInfo::getTags();
    }

    void DeprecatedAttribute::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }
}


