//
// Created by zhukovasky on 2020/8/6.
//

#include "RuntimeVisibleAnnotations.h"

ClassFile::RuntimeVisibleAnnotations::RuntimeVisibleAnnotations() {}

ClassFile::RuntimeVisibleAnnotations::~RuntimeVisibleAnnotations() {

}

u2 ClassFile::RuntimeVisibleAnnotations::getNumAnnotations() const {
    return num_annotations;
}

void ClassFile::RuntimeVisibleAnnotations::setNumAnnotations(u2 numAnnotations) {
    num_annotations = numAnnotations;
}

ClassFile::Annotation *ClassFile::RuntimeVisibleAnnotations::getAnnotations() const {
    return annotations;
}

void ClassFile::RuntimeVisibleAnnotations::setAnnotations(ClassFile::Annotation *annotations) {
    RuntimeVisibleAnnotations::annotations = annotations;
}

const list<ClassFile::Annotation *> &ClassFile::RuntimeVisibleAnnotations::getAnnotationsList() const {
    return annotationsList;
}

void ClassFile::RuntimeVisibleAnnotations::setAnnotationsList(const list<Annotation *> &annotationsList) {
    RuntimeVisibleAnnotations::annotationsList = annotationsList;
}

void ClassFile::RuntimeVisibleAnnotations::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::RuntimeVisibleAnnotations::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::RuntimeVisibleAnnotations::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::RuntimeVisibleAnnotations::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const string ClassFile::RuntimeVisibleAnnotations::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::RuntimeVisibleAnnotations::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
