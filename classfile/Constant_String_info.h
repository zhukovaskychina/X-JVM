//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_STRING_INFO_H
#define JVM_CONSTANT_STRING_INFO_H

#include "ConstantPools.h"

namespace ClassFile{
    class Constant_StringInfo :public ConstantPools{
    public:
        Constant_StringInfo();
        ~Constant_StringInfo();

        u2 getStringIndex() const;

        void setStringIndex(u2 stringIndex);

    private:
        u2 string_index;
    };
}



#endif //JVM_CONSTANT_STRING_INFO_H
