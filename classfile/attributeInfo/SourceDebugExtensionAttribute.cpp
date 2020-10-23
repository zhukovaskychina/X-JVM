//
// Created by zhukovasky on 2020/4/26.
//

#include "SourceDebugExtensionAttribute.h"

ClassFile::SourceDebugExtensionAttribute::SourceDebugExtensionAttribute() {}

ClassFile::SourceDebugExtensionAttribute::~SourceDebugExtensionAttribute() {

}

u1 *ClassFile::SourceDebugExtensionAttribute::getDebugExtension() const {
    return debug_extension;
}

void ClassFile::SourceDebugExtensionAttribute::setDebugExtension(u1 *debugExtension) {
    debug_extension = debugExtension;
}

void ClassFile::SourceDebugExtensionAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::SourceDebugExtensionAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::SourceDebugExtensionAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::SourceDebugExtensionAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::SourceDebugExtensionAttribute::getTags() const {
    return std::__cxx11::string(AttributesInfo::getTags());
}

void ClassFile::SourceDebugExtensionAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
