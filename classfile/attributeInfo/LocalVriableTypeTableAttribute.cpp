//
// Created by zhukovasky on 2020/4/26.
//

#include "LocalVriableTypeTableAttribute.h"

u2 ClassFile::LocalVariableTypeTableAttribute::getLocalVariableTypeTableLength() const {
    return local_variable_type_table_length;
}

void ClassFile::LocalVariableTypeTableAttribute::setLocalVariableTypeTableLength(u2 localVariableTypeTableLength) {
    local_variable_type_table_length = localVariableTypeTableLength;
}

ClassFile::LocalVariableTypeTable *ClassFile::LocalVariableTypeTableAttribute::getLocalVriableTypeTable() const {
    return localVriableTypeTable;
}

void ClassFile::LocalVariableTypeTableAttribute::setLocalVriableTypeTable(
        ClassFile::LocalVariableTypeTable *localVriableTypeTable) {
    LocalVariableTypeTableAttribute::localVriableTypeTable = localVriableTypeTable;
}

ClassFile::LocalVariableTypeTableAttribute::LocalVariableTypeTableAttribute() {}

ClassFile::LocalVariableTypeTableAttribute::~LocalVariableTypeTableAttribute() {

}
