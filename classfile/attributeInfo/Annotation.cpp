//
// Created by zhukovasky on 2020/8/6.
//

#include "Annotation.h"

ClassFile::Annotation::Annotation() {}

ClassFile::Annotation::~Annotation() {

}

u2 ClassFile::Annotation::getTypeIndex() const {
    return type_index;
}

void ClassFile::Annotation::setTypeIndex(u2 typeIndex) {
    type_index = typeIndex;
}

u2 ClassFile::Annotation::getNumElementValuePairs() const {
    return num_element_value_pairs;
}

void ClassFile::Annotation::setNumElementValuePairs(u2 numElementValuePairs) {
    num_element_value_pairs = numElementValuePairs;
}

ClassFile::ElementValuePairs *ClassFile::Annotation::getElementValuePairs() const {
    return elementValuePairs;
}

void ClassFile::Annotation::setElementValuePairs(ClassFile::ElementValuePairs *elementValuePairs) {
    Annotation::elementValuePairs = elementValuePairs;
}

const list<ClassFile::ElementValuePairs *> &ClassFile::Annotation::getElementValuePairsList() const {
    return elementValuePairsList;
}

void ClassFile::Annotation::setElementValuePairsList(const list<ElementValuePairs *> &elementValuePairsList) {
    Annotation::elementValuePairsList = elementValuePairsList;
}
