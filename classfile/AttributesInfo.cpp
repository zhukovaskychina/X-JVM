//
// Created by zhukovasky on 2020/4/23.
//

#include "AttributesInfo.h"

namespace ClassFile{
    AttributesInfo::AttributesInfo() {

    }
    AttributesInfo::~AttributesInfo() {

    }

    u2 AttributesInfo::getAttributeNameIndex() const {
        return this->attribute_name_index;
    }

    u4 AttributesInfo::getAttributeLength() const {
        return attribute_length;
    }

    void AttributesInfo::setAttributeNameIndex(u2 attributeNameIndex) {
        attribute_name_index = attributeNameIndex;
    }

    void AttributesInfo::setAttributeLength(u4 attributeLength) {
        attribute_length = attributeLength;
    }


    const std::string AttributesInfo::getTags() const {
        return this->tags;
    }

    void AttributesInfo::setTags(const std::string tags) {
        this->tags = tags;
    }
}