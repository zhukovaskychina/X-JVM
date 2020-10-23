//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_UTF8_INFO_H
#define JVM_CONSTANT_UTF8_INFO_H

#include "ConstantPools.h"

namespace ClassFile{
    class Constant_Utf8Info:public ConstantPools {
    public:
        Constant_Utf8Info();
        ~Constant_Utf8Info();

        u2 getLength() const;

        void setLength(u2 length);

        u1 *getBytes() const;

        void setBytes(u1 *bytes);

        std::string toString();

    private:
        u2 length;
        u1 *bytes;
    };
}



#endif //JVM_CONSTANT_UTF8_INFO_H
