//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_INSTRUCTIONS_H
#define JAVAP_INSTRUCTIONS_H


#include "../../runtime/JavaFrame.h"
#include "ByteCodeReader.h"

namespace Instruction{
    class Instructions {

    public:
        Instructions();

//从字节码中获取操作数
        virtual void fetchOperands(Instruction::ByteCodeReader *byteCodeReader)=0;

        //执行指令逻辑
        virtual void execute(Runtime::JavaFrame *javaFrame)=0;

        virtual ~Instructions();
    };

    class NoOperationInstruction:public Instructions{
    public:
        NoOperationInstruction();

    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        virtual ~NoOperationInstruction() override ;

        void execute(Runtime::JavaFrame *javaFrame) override;


    };
    //跳转指令，OffSet用于存储跳转偏移量
    class BranchInstruction:public Instructions{
    public:
        BranchInstruction()  ;

        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        virtual ~BranchInstruction() override ;

        int getOffset() const;

    private:
        int offset;

    };
    //存储和加载类指令需要根据索引存取局部变量表
    class Index8Instruction:public Instructions{
    public:
        Index8Instruction();

        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        virtual ~Index8Instruction() override ;

        u1 getIndex() const;

        void setIndex(u1 index);

    private:
        u1 index;
    };
    //有些指令需要访问运行时常量池，常量池索引由两字节操作数给出。
    class Index16Instruction:public Instructions{
    public:
        Index16Instruction();

        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        virtual ~Index16Instruction() override ;

        u2 getIndex() const;

        void setIndex(u2 index);

        void invokeMethod(Runtime::JavaFrame* javaFrame,Runtime::Heap::Method* method);

    private:
        u2 index;
    };

}



#endif //JAVAP_INSTRUCTIONS_H
