//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_INTEGER_INFO_H
#define JVM_CONSTANT_INTEGER_INFO_H

#include "../common/Internal.h"
#include "ConstantPools.h"

namespace ClassFile{
    class Constant_IntegerInfo:public ConstantPools {
    public:Constant_IntegerInfo();
        ~Constant_IntegerInfo();

        u4 getBytes() const;

        void setBytes(u4 bytes);

    private:
        u4 bytes;
    };

}


#endif //JVM_CONSTANT_INTEGER_INFO_H
