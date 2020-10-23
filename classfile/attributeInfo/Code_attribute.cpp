//
// Created by zhukovasky on 2020/4/26.
//

#include "Code_attribute.h"

namespace ClassFile{
    Code_attribute::Code_attribute(){

    }
    Code_attribute::~Code_attribute(){
        delete this->code;
        delete this->attributesInfo;
        delete this->exceptionTable;
    }

    const std::string Code_attribute::getTags() const {
        return AttributesInfo::getTags();
    }

    void Code_attribute::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }

    u2 Code_attribute::getAttributeNameIndex() const {
        return attribute_name_index;
    }

    void Code_attribute::setAttributeNameIndex(u2 attributeNameIndex) {
        attribute_name_index = attributeNameIndex;
    }

    u4 Code_attribute::getAttributeLength() const {
        return attribute_length;
    }

    void Code_attribute::setAttributeLength(u4 attributeLength) {
        attribute_length = attributeLength;
    }

    u2 Code_attribute::getMaxStack() const {
        return max_stack;
    }

    void Code_attribute::setMaxStack(u2 maxStack) {
        max_stack = maxStack;
    }

    u2 Code_attribute::getMaxLocals() const {
        return max_locals;
    }

    void Code_attribute::setMaxLocals(u2 maxLocals) {
        max_locals = maxLocals;
    }

    u4 Code_attribute::getCodeLength() const {
        return code_length;
    }

    void Code_attribute::setCodeLength(u4 codeLength) {
        code_length = codeLength;
    }

    u1 *Code_attribute::getCode() const {
        return code;
    }

    void Code_attribute::setCode(u1 *code) {
        Code_attribute::code = code;
    }

    u2 Code_attribute::getExceptionTableLength() const {
        return exception_table_length;
    }

    void Code_attribute::setExceptionTableLength(u2 exceptionTableLength) {
        exception_table_length = exceptionTableLength;
    }

    Exception_table **Code_attribute::getExceptionTable() const {
        return exceptionTable;
    }

    void Code_attribute::setExceptionTable(Exception_table **exceptionTable) {
        Code_attribute::exceptionTable = exceptionTable;
    }

    u2 Code_attribute::getAttributesCount() const {
        return attributes_count;
    }

    void Code_attribute::setAttributesCount(u2 attributesCount) {
        attributes_count = attributesCount;
    }

    AttributesInfo *Code_attribute::getAttributesInfo() const {
        return attributesInfo;
    }

    void Code_attribute::setAttributesInfo(AttributesInfo *attributesInfo) {
        Code_attribute::attributesInfo = attributesInfo;
    }

    const std::list<Exception_table *> &Code_attribute::getExceptionTableList() const {
        return exceptionTableList;
    }

    void Code_attribute::setExceptionTableList(const std::list<Exception_table *> &exceptionTableList) {
        Code_attribute::exceptionTableList = exceptionTableList;
    }

    const std::list<AttributesInfo *> &Code_attribute::getAttributesInfoList() const {
        return attributesInfoList;
    }

    void Code_attribute::setAttributesInfoList(const std::list<AttributesInfo *> &attributesInfoList) {
        Code_attribute::attributesInfoList = attributesInfoList;
    }
}