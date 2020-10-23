//
// Created by zhukovasky on 2020/8/6.
//

#include "BootstrapMethod.h"

ClassFile::BootstrapMethod::BootstrapMethod() {}

ClassFile::BootstrapMethod::~BootstrapMethod() {

}

u2 ClassFile::BootstrapMethod::getBootstrapMethodRef() const {
    return bootstrapMethodRef;
}

void ClassFile::BootstrapMethod::setBootstrapMethodRef(u2 bootstrapMethodRef) {
    BootstrapMethod::bootstrapMethodRef = bootstrapMethodRef;
}

u2 ClassFile::BootstrapMethod::getNumBootstrapArguments() const {
    return numBootstrapArguments;
}

void ClassFile::BootstrapMethod::setNumBootstrapArguments(u2 numBootstrapArguments) {
    BootstrapMethod::numBootstrapArguments = numBootstrapArguments;
}

u2 *ClassFile::BootstrapMethod::getBootstrapMethodArguments() const {
    return bootstrapMethodArguments;
}

void ClassFile::BootstrapMethod::setBootstrapMethodArguments(u2* bootstrapMethodArguments) {
    BootstrapMethod::bootstrapMethodArguments = bootstrapMethodArguments;
}
