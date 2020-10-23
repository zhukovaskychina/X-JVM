//
// Created by zhukovasky on 2020/8/6.
//

#include "BootstrapMethodAttribute.h"

ClassFile::BootstrapMethodAttribute::BootstrapMethodAttribute() {}

ClassFile::BootstrapMethodAttribute::~BootstrapMethodAttribute() {

}

u2 ClassFile::BootstrapMethodAttribute::getNumBootstrapMethods() const {
    return numBootstrapMethods;
}

void ClassFile::BootstrapMethodAttribute::setNumBootstrapMethods(u2 numBootstrapMethods) {
    BootstrapMethodAttribute::numBootstrapMethods = numBootstrapMethods;
}

ClassFile::BootstrapMethod* ClassFile::BootstrapMethodAttribute::getBootstrapMethod() const {
    return bootstrapMethod;
}

void ClassFile::BootstrapMethodAttribute::setBootstrapMethod(ClassFile::BootstrapMethod* bootstrapMethod) {
    BootstrapMethodAttribute::bootstrapMethod = bootstrapMethod;
}

const std::list<ClassFile::BootstrapMethod *> ClassFile::BootstrapMethodAttribute::getBootstrapMethodList() const {
    return bootstrapMethodList;
}

void
ClassFile::BootstrapMethodAttribute::setBootstrapMethodList(const std::list<BootstrapMethod *> &bootstrapMethodList) {
    BootstrapMethodAttribute::bootstrapMethodList = bootstrapMethodList;
}

void ClassFile::BootstrapMethodAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::BootstrapMethodAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::BootstrapMethodAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::BootstrapMethodAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::BootstrapMethodAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::BootstrapMethodAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
