//
// Created by zhukovasky on 2020/4/26.
//

#include "ElementValue.h"

namespace ClassFile{
    ElementValue::ElementValue() {}


    ElementValue::~ElementValue() {

    }

    u1 ElementValue::getTags() const {
        return tags;
    }

    void ElementValue::setTags(u1 tags) {
        ElementValue::tags = tags;
    }
}

