//
// Created by zhukovasky on 2020/4/26.
//

#ifndef JVM_CONSTANT_METHODHANDLE_INFO_H
#define JVM_CONSTANT_METHODHANDLE_INFO_H


#include "ConstantPools.h"

namespace ClassFile{
    class Constant_MethodHandleInfo:public ConstantPools{
    public:
        Constant_MethodHandleInfo();
        ~Constant_MethodHandleInfo();

        u1 getReferenceKind() const;

        void setReferenceKind(u1 referenceKind);

        u2 getReferenceIndex() const;

        void setReferenceIndex(u2 referenceIndex);

    private:
        u1 reference_kind;
        u2 reference_index;
    };
}




#endif //JVM_CONSTANT_METHODHANDLE_INFO_H
