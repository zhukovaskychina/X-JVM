//
// Created by zhukovasky on 2020/4/26.
//

#include "SourcFileAttribute.h"

ClassFile::SourceFileAttribute::SourceFileAttribute() {}

ClassFile::SourceFileAttribute::~SourceFileAttribute() {

}

u2 ClassFile::SourceFileAttribute::getSourcefile_index() const {
    return sourcefile_index;
}

void ClassFile::SourceFileAttribute::setSourcefile_index(u2 sourcefile_index) {
    SourceFileAttribute::sourcefile_index = sourcefile_index;
}

void ClassFile::SourceFileAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::SourceFileAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::SourceFileAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::SourceFileAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::SourceFileAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::SourceFileAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
