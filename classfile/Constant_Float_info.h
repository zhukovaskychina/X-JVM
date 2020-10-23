//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_FLOAT_INFO_H
#define JVM_CONSTANT_FLOAT_INFO_H

#include "ConstantPools.h"

namespace ClassFile{
    class Constant_FloatInfo :public ConstantPools{
    public:Constant_FloatInfo();
        ~Constant_FloatInfo();

        u4 getBytes() const;

        void setBytes(u4 bytes);

    private:
        u4 bytes;
    };
}



#endif //JVM_CONSTANT_FLOAT_INFO_H
