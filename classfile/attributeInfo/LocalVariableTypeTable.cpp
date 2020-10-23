//
// Created by zhukovasky on 2020/4/26.
//

#include "LocalVariableTypeTable.h"

ClassFile::LocalVariableTypeTable::LocalVariableTypeTable() {}

ClassFile::LocalVariableTypeTable::~LocalVariableTypeTable() {

}

u2 ClassFile::LocalVariableTypeTable::getStartPc() const {
    return start_pc;
}

void ClassFile::LocalVariableTypeTable::setStartPc(u2 startPc) {
    start_pc = startPc;
}

u2 ClassFile::LocalVariableTypeTable::getLength() const {
    return length;
}

void ClassFile::LocalVariableTypeTable::setLength(u2 length) {
    LocalVariableTypeTable::length = length;
}

u2 ClassFile::LocalVariableTypeTable::getNameIndex() const {
    return name_index;
}

void ClassFile::LocalVariableTypeTable::setNameIndex(u2 nameIndex) {
    name_index = nameIndex;
}

u2 ClassFile::LocalVariableTypeTable::getSignatureIndex() const {
    return signature_index;
}

void ClassFile::LocalVariableTypeTable::setSignatureIndex(u2 signatureIndex) {
    signature_index = signatureIndex;
}

u2 ClassFile::LocalVariableTypeTable::getIndex() const {
    return index;
}

void ClassFile::LocalVariableTypeTable::setIndex(u2 index) {
    LocalVariableTypeTable::index = index;
}
