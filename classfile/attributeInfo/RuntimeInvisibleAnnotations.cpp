//
// Created by zhukovasky on 2020/8/6.
//

#include "RuntimeInvisibleAnnotations.h"

u2 ClassFile::RuntimeInvisibleAnnotations::getNumAnnotations() const {
    return num_annotations;
}

void ClassFile::RuntimeInvisibleAnnotations::setNumAnnotations(u2 numAnnotations) {
    num_annotations = numAnnotations;
}

ClassFile::Annotation *ClassFile::RuntimeInvisibleAnnotations::getAnnotations() const {
    return annotations;
}

void ClassFile::RuntimeInvisibleAnnotations::setAnnotations(ClassFile::Annotation *annotations) {
    RuntimeInvisibleAnnotations::annotations = annotations;
}

ClassFile::RuntimeInvisibleAnnotations::RuntimeInvisibleAnnotations() {}

ClassFile::RuntimeInvisibleAnnotations::~RuntimeInvisibleAnnotations() {

}

void ClassFile::RuntimeInvisibleAnnotations::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::RuntimeInvisibleAnnotations::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::RuntimeInvisibleAnnotations::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::RuntimeInvisibleAnnotations::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const string ClassFile::RuntimeInvisibleAnnotations::getTags() const {
    return std::string(AttributesInfo::getTags());
}

void ClassFile::RuntimeInvisibleAnnotations::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
