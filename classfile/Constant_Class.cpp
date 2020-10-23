//
// Created by zhukovasky on 2020/4/26.
//

#include "Constant_Class.h"

namespace ClassFile{

    u2 Constant_Class::getNameIndex() const {
        return name_index;
    }

    void Constant_Class::setNameIndex(u2 nameIndex) {
        name_index = nameIndex;
    }

    Constant_Class::~Constant_Class() {
        free(reinterpret_cast<void *>(this->name_index));
    }

    Constant_Class::Constant_Class() {

    }


}