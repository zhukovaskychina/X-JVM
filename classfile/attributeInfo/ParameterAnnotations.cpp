//
// Created by zhukovasky on 2020/8/6.
//

#include "ParameterAnnotations.h"

ClassFile::ParameterAnnotations::ParameterAnnotations() {}

ClassFile::ParameterAnnotations::~ParameterAnnotations() {

}

u2 ClassFile::ParameterAnnotations::getNumAnnotations() const {
    return num_annotations;
}

void ClassFile::ParameterAnnotations::setNumAnnotations(u2 numAnnotations) {
    num_annotations = numAnnotations;
}

ClassFile::Annotation *ClassFile::ParameterAnnotations::getAnnotations() const {
    return annotations;
}

void ClassFile::ParameterAnnotations::setAnnotations(ClassFile::Annotation *annotations) {
    ParameterAnnotations::annotations = annotations;
}
