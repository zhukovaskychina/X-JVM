//
// Created by zhukovasky on 2020/4/26.
//

#include "LocalVariableTable.h"
namespace ClassFile{
    LocalVariableTable::LocalVariableTable() {}

    LocalVariableTable::~LocalVariableTable() {

    }

    u2 LocalVariableTable::getStart_pc() const {
        return start_pc;
    }

    void LocalVariableTable::setStart_pc(u2 start_pc) {
        LocalVariableTable::start_pc = start_pc;
    }

    u2 LocalVariableTable::getLength() const {
        return length;
    }

    void LocalVariableTable::setLength(u2 length) {
        LocalVariableTable::length = length;
    }

    u2 LocalVariableTable::getName_index() const {
        return name_index;
    }

    void LocalVariableTable::setName_index(u2 name_index) {
        LocalVariableTable::name_index = name_index;
    }

    u2 LocalVariableTable::getDescriptor_index() const {
        return descriptor_index;
    }

    void LocalVariableTable::setDescriptor_index(u2 descriptor_index) {
        LocalVariableTable::descriptor_index = descriptor_index;
    }

    u2 LocalVariableTable::getIndex() const {
        return index;
    }

    void LocalVariableTable::setIndex(u2 index) {
        LocalVariableTable::index = index;
    }
}