//
// Created by zhukovasky on 2020/8/6.
//

#include "ElementValuePairs.h"

ClassFile::ElementValuePairs::ElementValuePairs() {}

ClassFile::ElementValuePairs::~ElementValuePairs() {

}

u2 ClassFile::ElementValuePairs::getElementNameIndex() const {
    return element_name_index;
}

void ClassFile::ElementValuePairs::setElementNameIndex(u2 elementNameIndex) {
    element_name_index = elementNameIndex;
}

const ClassFile::ElementValue &ClassFile::ElementValuePairs::getElementValue() const {
    return elementValue;
}

void ClassFile::ElementValuePairs::setElementValue(const ClassFile::ElementValue &elementValue) {
    ElementValuePairs::elementValue = elementValue;
}
