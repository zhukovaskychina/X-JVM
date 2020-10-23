//
// Created by zhukovasky on 2020/8/7.
//

#include "StackMapTableAttributes.h"
namespace ClassFile{
    StackMapTableAttributes::StackMapTableAttributes() {}

    StackMapTableAttributes::~StackMapTableAttributes() {

    }

    u2 StackMapTableAttributes::getNumberOfEntries() const {
        return numberOfEntries;
    }

    void StackMapTableAttributes::setNumberOfEntries(u2 numberOfEntries) {
        StackMapTableAttributes::numberOfEntries = numberOfEntries;
    }

    StackMapFrame *StackMapTableAttributes::getStackMapFrame() const {
        return stackMapFrame;
    }

    void StackMapTableAttributes::setStackMapFrame(StackMapFrame *stackMapFrame) {
        StackMapTableAttributes::stackMapFrame = stackMapFrame;
    }

    const std::list<StackMapFrame *> &StackMapTableAttributes::getStackMapFrameList() const {
        return stackMapFrameList;
    }

    void StackMapTableAttributes::setStackMapFrameList(const std::list<StackMapFrame *> &stackMapFrameList) {
        StackMapTableAttributes::stackMapFrameList = stackMapFrameList;
    }

    void StackMapTableAttributes::setAttributeNameIndex(u2 attributeNameIndex) {
        AttributesInfo::setAttributeNameIndex(attributeNameIndex);
    }

    void StackMapTableAttributes::setAttributeLength(u4 attributeLength) {
        AttributesInfo::setAttributeLength(attributeLength);
    }

    u2 StackMapTableAttributes::getAttributeNameIndex() const {
        return AttributesInfo::getAttributeNameIndex();
    }

    u4 StackMapTableAttributes::getAttributeLength() const {
        return AttributesInfo::getAttributeLength();
    }

    const std::string StackMapTableAttributes::getTags() const {
        return std::__cxx11::string(AttributesInfo::getTags());
    }

    void StackMapTableAttributes::setTags(std::string tags) {
        AttributesInfo::setTags(tags);
    }
};