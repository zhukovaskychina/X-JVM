//
// Created by zhukovasky on 2020/8/7.
//

#ifndef JAVAP_STACKMAPFRAME_H
#define JAVAP_STACKMAPFRAME_H


#include "../../common/Internal.h"

namespace ClassFile{
    class VerificationTypeInfo{
    public:
        VerificationTypeInfo();

        virtual ~VerificationTypeInfo();

        u1 getTag() const;

        void setTag(u1 tag);

    private:
        u1 tag;

    };

    class TopVariableInfo:public VerificationTypeInfo{
    public:
        TopVariableInfo();

        virtual ~TopVariableInfo();

    };
    class IntegerVariableInfo:public VerificationTypeInfo{
    public:
        IntegerVariableInfo();

        virtual ~IntegerVariableInfo();

    };
    class FloatVariableInfo:public VerificationTypeInfo{
    public:
        FloatVariableInfo();

        virtual ~FloatVariableInfo();

    };
    class LongVariableInfo:public VerificationTypeInfo{
    public:
        LongVariableInfo();

        virtual ~LongVariableInfo();

    };
    class DoubleVariableInfo:public VerificationTypeInfo{
    public:
        DoubleVariableInfo();

        virtual ~DoubleVariableInfo();

    };
    class NullVariableInfo:public VerificationTypeInfo{
    public:
        NullVariableInfo();

        virtual ~NullVariableInfo();

    };
    class UninitializedThisVariableInfo:public VerificationTypeInfo{
    public:
        UninitializedThisVariableInfo();

        virtual ~UninitializedThisVariableInfo();

    };
    class ObjectVariableInfo:public VerificationTypeInfo{
    public:
        ObjectVariableInfo();

        virtual ~ObjectVariableInfo();

        u2 getCpoolIndex() const;

        void setCpoolIndex(u2 cpoolIndex);

    private:
        u2 cpoolIndex;
    };
    class UninitializedVariableInfo:public VerificationTypeInfo{
    public:
        UninitializedVariableInfo();

        virtual ~UninitializedVariableInfo();

    private:
        u2 offset;
    };

    class StackMapFrame {

    public:
        StackMapFrame();

        virtual ~StackMapFrame();

        u1 getFrameType() const;

        void setFrameType(u1 frameType);


    private:
        u1 frameType;
    };


    class SameFrame:public StackMapFrame{
    public:
        SameFrame();

        virtual ~SameFrame() override ;

    };

    class SameFrameExtend:public StackMapFrame{
    private:
        u2 offsetDelta;
    };

    class ChopFrame:public StackMapFrame{

    private:
        u2 offsetDelta;
    };
    class AppendFrame:public StackMapFrame{

    private:
        u2 offsetDelta;
        VerificationTypeInfo *stack;

    };


    class FullFrame:public StackMapFrame{
    private:
        u2 offsetDelta;
        u2 numberOfLocals;

    };
    class SameLocals1StackItemExtendedFrame:public StackMapFrame{
    private:
        u2 offsetDelta;
        VerificationTypeInfo* stack;
    };
    class SameLocals1StackItemFrame:public StackMapFrame{
    private:
        VerificationTypeInfo* stack;
    };

}


#endif //JAVAP_STACKMAPFRAME_H
