//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_CONST_H
#define JAVAP_CONST_H

#include "../base/Instructions.h"

namespace Instruction{
    class Const: public NoOperationInstruction {
    public:
         Const()= default;


        virtual ~Const()=0;

    };

    class ACONST_NULL:public NoOperationInstruction{
    public:

        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;


    };

    class DCONST_0:public NoOperationInstruction{
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class DCONST_1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FCONST_0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FCONST_1:public NoOperationInstruction{
    public:
        void fetchOperands(Instruction::ByteCodeReader *byteCodeReader) override;

        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class FCONST_2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ICONST_M1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };

    class ICONST_0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ICONST_1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ICONST_2:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;

    };
    class ICONST_3:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ICONST_4:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class ICONST_5:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

    class LCONST_0:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };
    class LCONST_1:public NoOperationInstruction{
    public:
        void execute(Runtime::JavaFrame *javaFrame) override;
    };

}


#endif //JAVAP_CONST_H
