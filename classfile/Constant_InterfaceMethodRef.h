//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_INTERFACEMETHODREF_H
#define JVM_CONSTANT_INTERFACEMETHODREF_H

#include "ConstantPools.h"

namespace ClassFile{
    class Constant_InterfaceMethodRef:public ConstantPools {
    public:
        Constant_InterfaceMethodRef();
        ~Constant_InterfaceMethodRef();

        u2 getClassIndex() const;

        void setClassIndex(u2 classIndex);

        u2 getNameAndTypeIndex() const;

        void setNameAndTypeIndex(u2 nameAndTypeIndex);

    private:
        u2 class_index;
        u2 name_and_type_index;

    };
}




#endif //JVM_CONSTANT_INTERFACEMETHODREF_H
