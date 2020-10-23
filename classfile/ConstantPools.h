//
// Created by zhukovasky on 2020/4/23.
//

#ifndef JVM_CONSTANTPOOLS_H
#define JVM_CONSTANTPOOLS_H

#include "../common/Internal.h"

namespace ClassFile{
    class ConstantPools{
    public:
        ConstantPools();
        virtual ~ConstantPools();


    public:
        u1 getTag() const;

        void setTag(u1 tag);

    private:
        //定义常量池索引
        u1 tag;
    };
}

#endif //JVM_CONSTANTPOOLS_H
