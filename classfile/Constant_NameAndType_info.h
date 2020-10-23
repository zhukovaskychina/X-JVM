//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_NAMEANDTYPE_INFO_H
#define JVM_CONSTANT_NAMEANDTYPE_INFO_H


#include "ConstantPools.h"

namespace ClassFile{
    class Constant_NameAndTypeInfo :public ConstantPools{
    public:
        Constant_NameAndTypeInfo();
        ~Constant_NameAndTypeInfo();

        u2 getNameIndex() const;

        void setNameIndex(u2 nameIndex);

        u2 getDescriptorIndex() const;

        void setDescriptorIndex(u2 descriptorIndex);

    private:
        u2 name_index;
        u2 descriptor_index;
    };

}


#endif //JVM_CONSTANT_NAMEANDTYPE_INFO_H
