//
// Created by zhukovasky on 2020/4/26.
//

#include "LocalVariableTableAttribute.h"

namespace ClassFile{
    LocalVariableTableAttribute::LocalVariableTableAttribute() {}

    LocalVariableTableAttribute::~LocalVariableTableAttribute() {

    }

    u2 LocalVariableTableAttribute::getLocal_variable_table_length() const {
        return local_variable_table_length;
    }

    void LocalVariableTableAttribute::setLocal_variable_table_length(u2 local_variable_table_length) {
        LocalVariableTableAttribute::local_variable_table_length = local_variable_table_length;
    }

    LocalVariableTable *LocalVariableTableAttribute::getLocalVariableTable() const {
        return localVariableTable;
    }

    void LocalVariableTableAttribute::setLocalVariableTable(LocalVariableTable *localVariableTable) {
        LocalVariableTableAttribute::localVariableTable = localVariableTable;
    }

    const std::list<LocalVariableTable *> &LocalVariableTableAttribute::getLocalVariableTableList() const {
        return localVariableTableList;
    }

    void LocalVariableTableAttribute::setLocalVariableTableList(
            const std::list<LocalVariableTable *> &localVariableTableList) {
        LocalVariableTableAttribute::localVariableTableList = localVariableTableList;
    }

    void LocalVariableTableAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
        AttributesInfo::setAttributeNameIndex(attributeNameIndex);
    }

    void LocalVariableTableAttribute::setAttributeLength(u4 attributeLength) {
        AttributesInfo::setAttributeLength(attributeLength);
    }

    u2 LocalVariableTableAttribute::getAttributeNameIndex() const {
        return AttributesInfo::getAttributeNameIndex();
    }

    u4 LocalVariableTableAttribute::getAttributeLength() const {
        return AttributesInfo::getAttributeLength();
    }

    const std::string LocalVariableTableAttribute::getTags() const {
        return AttributesInfo::getTags();
    }

    void LocalVariableTableAttribute::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }
}

