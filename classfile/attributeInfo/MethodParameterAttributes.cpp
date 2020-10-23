//
// Created by zhukovasky on 2020/8/6.
//

#include "MethodParameterAttributes.h"

ClassFile::MethodParameterAttributes::MethodParameterAttributes() {}

ClassFile::MethodParameterAttributes::~MethodParameterAttributes() {

}

u1 ClassFile::MethodParameterAttributes::getParameterCounts() const {
    return parameter_counts;
}

void ClassFile::MethodParameterAttributes::setParameterCounts(u1 parameterCounts) {
    parameter_counts = parameterCounts;
}

ClassFile::Parameters *ClassFile::MethodParameterAttributes::getParameters() const {
    return parameters;
}

void ClassFile::MethodParameterAttributes::setParameters(ClassFile::Parameters *parameters) {
    MethodParameterAttributes::parameters = parameters;
}

void ClassFile::MethodParameterAttributes::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::MethodParameterAttributes::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::MethodParameterAttributes::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::MethodParameterAttributes::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::MethodParameterAttributes::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::MethodParameterAttributes::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
