//
// Created by zhukovasky on 2020/8/13.
//

#include "Load.h"

namespace Instruction{
    static void _iload(Runtime::JavaFrame *javaFrame,u1 index){
        int value=javaFrame->getLocalVariableTables()->getInt(index-1);
        javaFrame->getOperandStack()->pushInt(value);
    }

    static void _aload(Runtime::JavaFrame *javaFrame,u1 index){
        Runtime::Object *value=javaFrame->getLocalVariableTables()->getRef(index-1);
        javaFrame->getOperandStack()->pushRef(value);
    }

    static void _fload(Runtime::JavaFrame *javaFrame,u1 index){
        float value=javaFrame->getLocalVariableTables()->getFloat(index-1);
        javaFrame->getOperandStack()->pushFloat(value);
    }
    static void _dload(Runtime::JavaFrame *javaFrame,u1 index){
        double value=javaFrame->getLocalVariableTables()->getDouble(index-1);
        javaFrame->getOperandStack()->pushDouble(value);
    }
    static void _lload(Runtime::JavaFrame *javaFrame,u1 index){
        long value=javaFrame->getLocalVariableTables()->getInt(index-1);
        javaFrame->getOperandStack()->pushLong(value);
    }

    void ILoad::execute(Runtime::JavaFrame *javaFrame) {
        _iload(javaFrame,this->getIndex());
    }

    void ILoad0::execute(Runtime::JavaFrame *javaFrame) {
        _iload(javaFrame,0);
    }
    void ILoad1::execute(Runtime::JavaFrame *javaFrame) {
        _iload(javaFrame,1);
    }
    void ILoad2::execute(Runtime::JavaFrame *javaFrame) {
        _iload(javaFrame,2);
    }
    void ILoad3::execute(Runtime::JavaFrame *javaFrame) {
        _iload(javaFrame,3);
    }
    void ALoad::execute(Runtime::JavaFrame *javaFrame) {
        _aload(javaFrame,this->getIndex());
    }
    void ALoad0::execute(Runtime::JavaFrame *javaFrame) {
        _aload(javaFrame,0);
    }
    void ALoad1::execute(Runtime::JavaFrame *javaFrame) {
        _aload(javaFrame,1);
    }
    void ALoad2::execute(Runtime::JavaFrame *javaFrame) {
        _aload(javaFrame,2);
    }
    void ALoad3::execute(Runtime::JavaFrame *javaFrame) {
        _aload(javaFrame,3);
    }

    void LLoad::execute(Runtime::JavaFrame *javaFrame) {
        _lload(javaFrame,this->getIndex());
    }
    void LLoad0::execute(Runtime::JavaFrame *javaFrame) {
        _lload(javaFrame,0);
    }
    void LLoad1::execute(Runtime::JavaFrame *javaFrame) {
        _lload(javaFrame,1);
    }
    void LLoad2::execute(Runtime::JavaFrame *javaFrame) {
        _lload(javaFrame,2);
    }
    void LLoad3::execute(Runtime::JavaFrame *javaFrame) {
        _lload(javaFrame,3);
    }
    void FLoad::execute(Runtime::JavaFrame *javaFrame) {
        _fload(javaFrame,this->getIndex());
    }
    void FLoad0::execute(Runtime::JavaFrame *javaFrame) {
        _fload(javaFrame,0);
    }
    void FLoad1::execute(Runtime::JavaFrame *javaFrame) {
        _fload(javaFrame,1);
    }
    void FLoad2::execute(Runtime::JavaFrame *javaFrame) {
        _fload(javaFrame,2);
    }
    void FLoad3::execute(Runtime::JavaFrame *javaFrame) {
        _fload(javaFrame,3);
    }
    void DLoad::execute(Runtime::JavaFrame *javaFrame) {
        _dload(javaFrame,this->getIndex());
    }
    void DLoad0::execute(Runtime::JavaFrame *javaFrame) {
        _dload(javaFrame,0);
    }
    void DLoad1::execute(Runtime::JavaFrame *javaFrame) {
        _dload(javaFrame,1);
    }
    void DLoad2::execute(Runtime::JavaFrame *javaFrame) {
        _dload(javaFrame,2);
    }
    void DLoad3::execute(Runtime::JavaFrame *javaFrame) {
        _dload(javaFrame,3);
    }

}