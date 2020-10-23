//
// Created by zhukovasky on 2020/8/13.
//

#include "Store.h"
namespace Instruction{
    static void _iStore(Runtime::JavaFrame *javaFrame,u1 index){
        int value=javaFrame->getOperandStack()->popInt();
        javaFrame->getLocalVariableTables()->setInt(index,value);

    }

    static void _aStore(Runtime::JavaFrame *javaFrame,u1 index){
        Runtime::Object *value=javaFrame->getOperandStack()->popObject();
        javaFrame->getLocalVariableTables()->setRefs(index,value);

    }

    static void _fStore(Runtime::JavaFrame *javaFrame,u1 index){
        float value=javaFrame->getOperandStack()->popFloat();
        javaFrame->getLocalVariableTables()->setFloat(index,value);
    }
    static void _dStore(Runtime::JavaFrame *javaFrame,u1 index){
        double value=javaFrame->getOperandStack()->popDouble();
        javaFrame->getLocalVariableTables()->setDouble(index,value);
    }
    static void _lStore(Runtime::JavaFrame *javaFrame,u1 index){
        long value=javaFrame->getOperandStack()->popLong();
        javaFrame->getLocalVariableTables()->setLong(index,value);
    }

    void IStore::execute(Runtime::JavaFrame *javaFrame) {
        _iStore(javaFrame,this->getIndex());
    }

    void IStore0::execute(Runtime::JavaFrame *javaFrame) {
        _iStore(javaFrame,0);
    }
    void IStore1::execute(Runtime::JavaFrame *javaFrame) {
        _iStore(javaFrame,1);
    }
    void IStore2::execute(Runtime::JavaFrame *javaFrame) {
        _iStore(javaFrame,2);
    }
    void IStore3::execute(Runtime::JavaFrame *javaFrame) {
        _iStore(javaFrame,3);
    }
    void AStore::execute(Runtime::JavaFrame *javaFrame) {
        _aStore(javaFrame,this->getIndex());
    }
    void AStore0::execute(Runtime::JavaFrame *javaFrame) {
        _aStore(javaFrame,0);
    }
    void AStore1::execute(Runtime::JavaFrame *javaFrame) {
        _aStore(javaFrame,1);
    }
    void AStore2::execute(Runtime::JavaFrame *javaFrame) {
        _aStore(javaFrame,2);
    }
    void AStore3::execute(Runtime::JavaFrame *javaFrame) {
        _aStore(javaFrame,3);
    }

    void LStore::execute(Runtime::JavaFrame *javaFrame) {
        _lStore(javaFrame,this->getIndex());
    }
    void LStore0::execute(Runtime::JavaFrame *javaFrame) {
        _lStore(javaFrame,0);
    }
    void LStore1::execute(Runtime::JavaFrame *javaFrame) {
        _lStore(javaFrame,1);
    }
    void LStore2::execute(Runtime::JavaFrame *javaFrame) {
        _lStore(javaFrame,2);
    }
    void LStore3::execute(Runtime::JavaFrame *javaFrame) {
        _lStore(javaFrame,3);
    }
    void FStore::execute(Runtime::JavaFrame *javaFrame) {
        _fStore(javaFrame,this->getIndex());
    }
    void FStore0::execute(Runtime::JavaFrame *javaFrame) {
        _fStore(javaFrame,0);
    }
    void FStore1::execute(Runtime::JavaFrame *javaFrame) {
        _fStore(javaFrame,1);
    }
    void FStore2::execute(Runtime::JavaFrame *javaFrame) {
        _fStore(javaFrame,2);
    }
    void FStore3::execute(Runtime::JavaFrame *javaFrame) {
        _fStore(javaFrame,3);
    }
    void DStore::execute(Runtime::JavaFrame *javaFrame) {
        _dStore(javaFrame,this->getIndex());
    }
    void DStore0::execute(Runtime::JavaFrame *javaFrame) {
        _dStore(javaFrame,0);
    }
    void DStore1::execute(Runtime::JavaFrame *javaFrame) {
        _dStore(javaFrame,1);
    }
    void DStore2::execute(Runtime::JavaFrame *javaFrame) {
        _dStore(javaFrame,2);
    }
    void DStore3::execute(Runtime::JavaFrame *javaFrame) {
        _dStore(javaFrame,3);
    }

}