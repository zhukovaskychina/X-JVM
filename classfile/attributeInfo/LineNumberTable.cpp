//
// Created by zhukovasky on 2020/4/26.
//

#include "LineNumberTable.h"

ClassFile::LineNumberTable::LineNumberTable() {}

ClassFile::LineNumberTable::~LineNumberTable() {

}

u2 ClassFile::LineNumberTable::getStart_pc() const {
    return start_pc;
}

void ClassFile::LineNumberTable::setStart_pc(u2 start_pc) {
    LineNumberTable::start_pc = start_pc;
}

u2 ClassFile::LineNumberTable::getLine_number() const {
    return line_number;
}

void ClassFile::LineNumberTable::setLine_number(u2 line_number) {
    LineNumberTable::line_number = line_number;
}
