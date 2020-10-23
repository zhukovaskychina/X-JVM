//
// Created by zhukovasky on 2020/8/27.
//

#ifndef JVM_CONSTANTPOOLSLIST_H
#define JVM_CONSTANTPOOLSLIST_H


#include <list>
#include <array>
#include <vector>
#include "ConstantPools.h"

namespace ClassFile{
    class ConstantPoolsList {

    public:
        ConstantPoolsList(const std::list<ConstantPools *> &constantPoolList);

        ConstantPoolsList();

        void setConstantPoolsInstance(u2 index,ConstantPools* object);
        ConstantPools* getIndex(u2 index);

        virtual ~ConstantPoolsList();

        int getSize();

        void clear();
    private:
        std::vector<ConstantPools*> array;
        std::list<ConstantPools*> constantPoolList;
    };
}



#endif //JVM_CONSTANTPOOLSLIST_H
