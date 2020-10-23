//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_CLASS_H
#define JVM_CONSTANT_CLASS_H

#include <string>
#include "ConstantPools.h"

namespace ClassFile {
    class Constant_Class : public ConstantPools {
    public:
        Constant_Class();

        virtual ~Constant_Class();

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

    private:
        //常量池索引
        u2 name_index;
    };
}


#endif //JVM_CONSTANT_CLASS_H
