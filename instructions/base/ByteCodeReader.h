//
// Created by zhukovasky on 2020/8/13.
//

#ifndef JAVAP_BYTECODEREADER_H
#define JAVAP_BYTECODEREADER_H


#include "../../common/Internal.h"

namespace Instruction{
    class ByteCodeReader {
    public:
        ByteCodeReader();

        ByteCodeReader(int pc, u1 *code);

        ByteCodeReader(u1 *code);


        int getPc() const;

        void setPc(int pc);

        u1 *getCode() const;

        void setCode(u1 *code);

        virtual ~ByteCodeReader();

        u1 readU1();

        u2 readU2();

        u4 read4();

        void reset(u1* code,int pc);
    private:
        int pc;
        u1* code; //字节码
    };

}



#endif //JAVAP_BYTECODEREADER_H
