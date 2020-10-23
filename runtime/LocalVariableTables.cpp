//
// Created by zhukovasky on 2020/8/30.
//

#include "LocalVariableTables.h"
namespace Runtime{
    LocalVariableTables::LocalVariableTables() {}

    LocalVariableTables::LocalVariableTables(int maxLocals) : maxLocals(maxLocals) {
        this->maxLocals=maxLocals;
    }

    LocalVariableTables::~LocalVariableTables() {
        this->localVars.clear();
    }

    int LocalVariableTables::getInt(long index) {
        Slots* currentSlots=this->localVars[index];
        return currentSlots->getNums();
    }

    void LocalVariableTables::setInt(long index, int value) {
        if(index>this->maxLocals){
            //抛出异常

        }
        Slots* slots=new Slots();
        slots->setNums(value);
        this->localVars.push_back(slots);
    }

    float LocalVariableTables::getFloat(long index) {
        Slots* slots=this->localVars[index];
        return slots->getFloatValue();
    }

    void LocalVariableTables::setFloat(long index, float value) {
        if(index>this->maxLocals){
            //抛出异常

        }
        Slots* slots=new Slots();
        slots->setNums(value);
        this->localVars.push_back(slots);
    }

    long LocalVariableTables::getLong(long index) {
        return this->localVars[index]->getNums();
    }

    void LocalVariableTables::setLong(long index, long value) {
        if(index>this->maxLocals){
            //抛出异常

        }
        Slots* slots=new Slots();
        slots->setNums(value);
        this->localVars.push_back(slots);
    }

    double LocalVariableTables::getDouble(long index) {
        return this->localVars[index]->getDoubleValue();
    }

    void LocalVariableTables::setDouble(long index, double value) {
        if(index>this->maxLocals){
            //抛出异常

        }
        Slots* slots=new Slots();
        slots->setNums(value);
        this->localVars.push_back(slots);
    }

    Object *LocalVariableTables::getRef(long index) {
        return this->localVars[index]->getRefs();
    }

    void LocalVariableTables::setRefs(long index, Object *refs) {
        if(index>this->maxLocals){
            //抛出异常

        }
        Slots* slots=new Slots();
        slots->setRefs(refs);
        this->localVars.push_back(slots);
    }

    void LocalVariableTables::setSlots(long index, Slots *slots) {
        this->localVars.push_back(slots);
    }

    Slots *LocalVariableTables::getSlots(long index) {
        return this->localVars[index];
    }
}