//
// Created by zhukovasky on 2020/8/6.
//

#include "RuntimeInvisibleParameterAnnotationAttributes.h"

ClassFile::RuntimeInvisibleParameterAnnotationAttributes::RuntimeInvisibleParameterAnnotationAttributes() {}

ClassFile::RuntimeInvisibleParameterAnnotationAttributes::~RuntimeInvisibleParameterAnnotationAttributes() {

}

u1 ClassFile::RuntimeInvisibleParameterAnnotationAttributes::getNumParameters() const {
    return numParameters;
}

void ClassFile::RuntimeInvisibleParameterAnnotationAttributes::setNumParameters(u1 numParameters) {
    RuntimeInvisibleParameterAnnotationAttributes::numParameters = numParameters;
}

ClassFile::ParameterAnnotations *
ClassFile::RuntimeInvisibleParameterAnnotationAttributes::getParameterAnnotations() const {
    return parameterAnnotations;
}

void ClassFile::RuntimeInvisibleParameterAnnotationAttributes::setParameterAnnotations(
        ClassFile::ParameterAnnotations *parameterAnnotations) {
    RuntimeInvisibleParameterAnnotationAttributes::parameterAnnotations = parameterAnnotations;
}

void ClassFile::RuntimeInvisibleParameterAnnotationAttributes::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::RuntimeInvisibleParameterAnnotationAttributes::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::RuntimeInvisibleParameterAnnotationAttributes::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::RuntimeInvisibleParameterAnnotationAttributes::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const string ClassFile::RuntimeInvisibleParameterAnnotationAttributes::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::RuntimeInvisibleParameterAnnotationAttributes::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
