//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_METHODTYPE_INFO_H
#define JVM_CONSTANT_METHODTYPE_INFO_H


#include "ConstantPools.h"

namespace ClassFile{
    class Constant_MethodTypeInfo:public ConstantPools{
    public:
        Constant_MethodTypeInfo();
        ~Constant_MethodTypeInfo();

        u2 getDescriptorIndex() const;

        void setDescriptorIndex(u2 descriptorIndex);

    private:
        u2 descriptor_index;
    };
}



#endif //JVM_CONSTANT_METHODTYPE_INFO_H
