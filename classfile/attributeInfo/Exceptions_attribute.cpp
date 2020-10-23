//
// Created by zhukovasky on 2020/4/26.
//

#include "Exceptions_attribute.h"

ClassFile::Exceptions_attribute::Exceptions_attribute() {}

ClassFile::Exceptions_attribute::~Exceptions_attribute() {

}

u2 ClassFile::Exceptions_attribute::getNumberOfExceptions() const {
    return number_of_exceptions;
}

void ClassFile::Exceptions_attribute::setNumberOfExceptions(u2 numberOfExceptions) {
    number_of_exceptions = numberOfExceptions;
}

u2 *ClassFile::Exceptions_attribute::getExceptionIndexTable() const {
    return exceptionIndexTable;
}

void ClassFile::Exceptions_attribute::setExceptionIndexTable(u2 *exceptionIndexTable) {
    Exceptions_attribute::exceptionIndexTable = exceptionIndexTable;
}

void ClassFile::Exceptions_attribute::setAttributeNameIndex(u2 attributeNameIndex) {
    AttributesInfo::setAttributeNameIndex(attributeNameIndex);
}

void ClassFile::Exceptions_attribute::setAttributeLength(u4 attributeLength) {
    AttributesInfo::setAttributeLength(attributeLength);
}

u2 ClassFile::Exceptions_attribute::getAttributeNameIndex() const {
    return AttributesInfo::getAttributeNameIndex();
}

u4 ClassFile::Exceptions_attribute::getAttributeLength() const {
    return AttributesInfo::getAttributeLength();
}

const std::string ClassFile::Exceptions_attribute::getTags() const {
    return AttributesInfo::getTags();
}

void ClassFile::Exceptions_attribute::setTags(std::string tags) {
    AttributesInfo::setTags(tags);
}
