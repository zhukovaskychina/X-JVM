//
// Created by zhukovasky on 2020/4/26.
//

#include "InnerClasses.h"

ClassFile::InnerClasses::InnerClasses() {}

ClassFile::InnerClasses::~InnerClasses() {

}

u2 ClassFile::InnerClasses::getNumberOfClasses() const {
    return number_of_classes;
}

void ClassFile::InnerClasses::setNumberOfClasses(u2 numberOfClasses) {
    number_of_classes = numberOfClasses;
}

ClassessInner *ClassFile::InnerClasses::getClassess() const {
    return classess;
}

void ClassFile::InnerClasses::setClassess(ClassessInner *classess) {
    InnerClasses::classess = classess;
}

void ClassFile::InnerClasses::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::InnerClasses::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::InnerClasses::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::InnerClasses::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::InnerClasses::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::InnerClasses::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
