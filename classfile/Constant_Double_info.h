//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_DOUBLE_INFO_H
#define JVM_CONSTANT_DOUBLE_INFO_H

#include "ConstantPools.h"

namespace ClassFile{
    class Constant_DoubleInfo:public ConstantPools {
    public:
        Constant_DoubleInfo();
        virtual ~Constant_DoubleInfo();

        u4 getHighBytes() const;

        void setHighBytes(u4 highBytes);

        u4 getLowBytes() const;

        void setLowBytes(u4 lowBytes);

    private:
        u4 high_bytes;
        u4 low_bytes;
    };

}


#endif //JVM_CONSTANT_DOUBLE_INFO_H
