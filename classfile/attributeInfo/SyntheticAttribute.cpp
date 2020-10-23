//
// Created by zhukovasky on 2020/4/26.
//

#include "SyntheticAttribute.h"

ClassFile::SyntheticAttribute::SyntheticAttribute() {}

ClassFile::SyntheticAttribute::~SyntheticAttribute() {

}

void ClassFile::SyntheticAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::SyntheticAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::SyntheticAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::SyntheticAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::SyntheticAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::SyntheticAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
