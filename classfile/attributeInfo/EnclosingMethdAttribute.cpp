//
// Created by zhukovasky on 2020/4/26.
//

#include "EnclosingMethdAttribute.h"

ClassFile::EnclosingMethodAttribute::EnclosingMethodAttribute() {}

ClassFile::EnclosingMethodAttribute::~EnclosingMethodAttribute() {

}

u2 ClassFile::EnclosingMethodAttribute::getClassIndex() const {
    return class_index;
}

void ClassFile::EnclosingMethodAttribute::setClassIndex(u2 classIndex) {
    class_index = classIndex;
}

u2 ClassFile::EnclosingMethodAttribute::getMethodIndex() const {
    return method_index;
}

void ClassFile::EnclosingMethodAttribute::setMethodIndex(u2 methodIndex) {
    method_index = methodIndex;
}

void ClassFile::EnclosingMethodAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::EnclosingMethodAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::EnclosingMethodAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::EnclosingMethodAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::EnclosingMethodAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::EnclosingMethodAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
