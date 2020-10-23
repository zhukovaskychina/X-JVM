//
// Created by zhukovasky on 2020/4/26.
//

#include "LineNumberTableAttribute.h"
#include "LineNumberTable.h"
ClassFile::LineNumberTableAttribute::LineNumberTableAttribute() {}

ClassFile::LineNumberTableAttribute::~LineNumberTableAttribute() {

}

u2 ClassFile::LineNumberTableAttribute::getLineNumberTableLength() const {
    return line_number_table_length;
}

void ClassFile::LineNumberTableAttribute::setLineNumberTableLength(u2 lineNumberTableLength) {
    line_number_table_length = lineNumberTableLength;
}

ClassFile::LineNumberTable *ClassFile::LineNumberTableAttribute::getLineNumberTable() const {
    return lineNumberTable;
}

void ClassFile::LineNumberTableAttribute::setLineNumberTable(ClassFile::LineNumberTable *lineNumberTable) {
    LineNumberTableAttribute::lineNumberTable = lineNumberTable;
}

const std::list<ClassFile::LineNumberTable*> ClassFile::LineNumberTableAttribute::getLineNumberTableList() const {
    return lineNumberTableList;
}

void
ClassFile::LineNumberTableAttribute::setLineNumberTableList(const std::list<LineNumberTable *> &lineNumberTableList) {
    LineNumberTableAttribute::lineNumberTableList = lineNumberTableList;
}

void ClassFile::LineNumberTableAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::LineNumberTableAttribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::LineNumberTableAttribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::LineNumberTableAttribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::LineNumberTableAttribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::LineNumberTableAttribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
