//
// Created by zhukovasky on 2020/8/7.
//

#include "AnnotationDefaultAttribute.h"
namespace ClassFile{
    AnnotationDefaultAttribute::AnnotationDefaultAttribute() {}

    AnnotationDefaultAttribute::~AnnotationDefaultAttribute() {
        delete defaultValue;
    }

    ElementValue *AnnotationDefaultAttribute::getDefaultValue() const {
        return defaultValue;
    }

    void AnnotationDefaultAttribute::setDefaultValue(ElementValue *defaultValue) {
        AnnotationDefaultAttribute::defaultValue = defaultValue;
    }

    void AnnotationDefaultAttribute::setAttributeNameIndex(u2 attributeNameIndex) {
        AttributesInfo::setAttributeNameIndex(attributeNameIndex);
    }

    void AnnotationDefaultAttribute::setAttributeLength(u4 attributeLength) {
        AttributesInfo::setAttributeLength(attributeLength);
    }

    u2 AnnotationDefaultAttribute::getAttributeNameIndex() const {
        return AttributesInfo::getAttributeNameIndex();
    }

    u4 AnnotationDefaultAttribute::getAttributeLength() const {
        return AttributesInfo::getAttributeLength();
    }

    const string AnnotationDefaultAttribute::getTags() const {
        return std::__cxx11::string(AttributesInfo::getTags());
    }

    void AnnotationDefaultAttribute::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }
}