//
// Created by zhukovasky on 2020/8/27.
//

#include "ConstantPoolsList.h"
namespace ClassFile{
    ConstantPoolsList::ConstantPoolsList(const std::list<ConstantPools *> &constantPoolList) : constantPoolList(
            constantPoolList) {
        this->constantPoolList=constantPoolList;

    }

    ConstantPoolsList::ConstantPoolsList() {
        this->constantPoolList.clear();
        this->constantPoolList.push_back(nullptr);
        this->array.clear();
        this->array.push_back(nullptr);
    }

    ConstantPoolsList::~ConstantPoolsList() {
        this->constantPoolList.clear();
        this->array.clear();
    }

    void ConstantPoolsList::setConstantPoolsInstance(u2 index, ConstantPools *object) {
        this->constantPoolList.push_back(object);
        this->array.push_back(object);
        this->array[index]=(object);
    }

    ConstantPools *ConstantPoolsList::getIndex(u2 index) {
        return this->array[index];
    }

    int ConstantPoolsList::getSize() {
        return this->array.size();
    }

    void ConstantPoolsList::clear() {
        this->array.clear();
        this->constantPoolList.clear();
        delete this;
    }
}