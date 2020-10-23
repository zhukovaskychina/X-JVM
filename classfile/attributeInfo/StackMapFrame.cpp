//
// Created by zhukovasky on 2020/8/7.
//

#include <malloc.h>
#include "StackMapFrame.h"

namespace ClassFile{
    StackMapFrame::StackMapFrame() {}

    StackMapFrame::~StackMapFrame() {

    }

    u1 StackMapFrame::getFrameType() const {
        return frameType;
    }

    void StackMapFrame::setFrameType(u1 frameType) {
        StackMapFrame::frameType = frameType;
    }

    SameFrame::SameFrame() {}

    SameFrame::~SameFrame()  {

    }



    VerificationTypeInfo::VerificationTypeInfo() {}

    VerificationTypeInfo::~VerificationTypeInfo() {

    }

    u1 VerificationTypeInfo::getTag() const {
        return tag;
    }

    void VerificationTypeInfo::setTag(u1 tag) {
        VerificationTypeInfo::tag = tag;
    }

    FloatVariableInfo::FloatVariableInfo() {}

    FloatVariableInfo::~FloatVariableInfo() {

    }

    IntegerVariableInfo::IntegerVariableInfo() {}

    IntegerVariableInfo::~IntegerVariableInfo() {

    }

    DoubleVariableInfo::DoubleVariableInfo() {}

    DoubleVariableInfo::~DoubleVariableInfo() {

    }

    LongVariableInfo::LongVariableInfo() {}

    LongVariableInfo::~LongVariableInfo() {

    }

    NullVariableInfo::NullVariableInfo() {}

    NullVariableInfo::~NullVariableInfo() {

    }

    TopVariableInfo::TopVariableInfo() {}

    TopVariableInfo::~TopVariableInfo() {

    }

    UninitializedVariableInfo::UninitializedVariableInfo() {}

    UninitializedVariableInfo::~UninitializedVariableInfo() {

    }

    UninitializedThisVariableInfo::UninitializedThisVariableInfo() {}

    UninitializedThisVariableInfo::~UninitializedThisVariableInfo() {

    }

    ObjectVariableInfo::ObjectVariableInfo() {}

    ObjectVariableInfo::~ObjectVariableInfo() {
        free(reinterpret_cast<void *>(ObjectVariableInfo::cpoolIndex));
    }

    u2 ObjectVariableInfo::getCpoolIndex() const {
        return cpoolIndex;
    }

    void ObjectVariableInfo::setCpoolIndex(u2 cpoolIndex) {
        ObjectVariableInfo::cpoolIndex = cpoolIndex;
    }
}