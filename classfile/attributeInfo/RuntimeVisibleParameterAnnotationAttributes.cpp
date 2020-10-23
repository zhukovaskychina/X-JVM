//
// Created by zhukovasky on 2020/8/6.
//

#include "RuntimeVisibleParameterAnnotationAttributes.h"

ClassFile::RuntimeVisibleParameterAnnotationAttributes::RuntimeVisibleParameterAnnotationAttributes() {}

ClassFile::RuntimeVisibleParameterAnnotationAttributes::~RuntimeVisibleParameterAnnotationAttributes() {

}

u1 ClassFile::RuntimeVisibleParameterAnnotationAttributes::getNumParameters() const {
    return numParameters;
}

void ClassFile::RuntimeVisibleParameterAnnotationAttributes::setNumParameters(u1 numParameters) {
    RuntimeVisibleParameterAnnotationAttributes::numParameters = numParameters;
}

ClassFile::ParameterAnnotations *
ClassFile::RuntimeVisibleParameterAnnotationAttributes::getParameterAnnotations() const {
    return parameterAnnotations;
}

void ClassFile::RuntimeVisibleParameterAnnotationAttributes::setParameterAnnotations(
        ClassFile::ParameterAnnotations *parameterAnnotations) {
    RuntimeVisibleParameterAnnotationAttributes::parameterAnnotations = parameterAnnotations;
}

void ClassFile::RuntimeVisibleParameterAnnotationAttributes::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::RuntimeVisibleParameterAnnotationAttributes::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::RuntimeVisibleParameterAnnotationAttributes::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::RuntimeVisibleParameterAnnotationAttributes::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const string ClassFile::RuntimeVisibleParameterAnnotationAttributes::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::RuntimeVisibleParameterAnnotationAttributes::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
