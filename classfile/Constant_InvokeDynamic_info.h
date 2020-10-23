//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_INVOKEDYNAMIC_INFO_H
#define JVM_CONSTANT_INVOKEDYNAMIC_INFO_H

#include "ConstantPools.h"

namespace ClassFile{

    class Constant_InvokeDynamicInfo:public ConstantPools{
    public:
        Constant_InvokeDynamicInfo();
        ~Constant_InvokeDynamicInfo();

        u2 getBootstrapMethodAttrIndex() const;

        void setBootstrapMethodAttrIndex(u2 bootstrapMethodAttrIndex);

        u2 getNameAndTypeIndex() const;

        void setNameAndTypeIndex(u2 nameAndTypeIndex);

    private:
        u2 bootstrap_method_attr_index;
        u2 name_and_type_index;
    };

}


#endif //JVM_CONSTANT_INVOKEDYNAMIC_INFO_H
