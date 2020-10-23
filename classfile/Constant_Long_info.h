//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_LONG_INFO_H
#define JVM_CONSTANT_LONG_INFO_H


#include "ConstantPools.h"

namespace ClassFile{
    class Constant_LongInfo:public ConstantPools {
    public:
        Constant_LongInfo();
        ~Constant_LongInfo();

        u4 getHighBytes() const;

        void setHighBytes(u4 highBytes);

        u4 getLowBytes() const;

        void setLowBytes(u4 lowBytes);

    private:
        u4 high_bytes;
        u4 low_bytes;
    };
}





#endif //JVM_CONSTANT_LONG_INFO_H
