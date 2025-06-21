//
// Created by zhukovasky on 2020/4/26.
//

#include "ConstantValue.h"
#include "../../utils/PlatformCompat.h"


namespace ClassFile{

    ConstantValue::ConstantValue() {}

    ConstantValue::~ConstantValue() {
        // this->constantvalue_index;
    }



    u2 ConstantValue::getConstantvalueIndex() const {
        return this->constantvalue_index;
    }

    void ConstantValue::setConstantvalueIndex(u2 constantvalueIndex) {
        this->constantvalue_index=constantvalueIndex;
    }

    void ConstantValue::setAttributeNameIndex(u2 attributeNameIndex) {
        AttributesInfo::setAttributeNameIndex(attributeNameIndex);
    }

    void ConstantValue::setAttributeLength(u4 attributeLength) {
        AttributesInfo::setAttributeLength(attributeLength);
    }

    u2 ConstantValue::getAttributeNameIndex() const {
        return AttributesInfo::getAttributeNameIndex();
    }

    u4 ConstantValue::getAttributeLength() const {
        return AttributesInfo::getAttributeLength();
    }

    std::string ConstantValue::toString() {
        return std::string(AttributesInfo::getTags());
    }

    const std::string ConstantValue::getTags() const {
        return AttributesInfo::getTags();
    }

    void ConstantValue::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }
}
