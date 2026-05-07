//
// Created by zhukovasky on 2020/8/11.
//

#include "OperandStack.h"

namespace Runtime{

    OperandStack *OperandStack::newOperandStack(int maxSize) {

        OperandStack *operandStack=new OperandStack();
        operandStack->size=0;
        operandStack->maxSize=maxSize;
        return operandStack;
    }

    void OperandStack::pushInt(int value) {
        Slots *slots=new Slots;
        slots->setNums(value);

        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    int OperandStack::popInt() {
        this->size=this->size-1;
        int value=this->innerSlotsList[this->size]->getNums();
        this->innerSlotsList.pop_back();
        return value;
    }

    void OperandStack::pushLong(long value) {
        Slots *slots=new Slots;
        slots->setNums(value);
        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    long OperandStack::popLong() {
        this->size=this->size-1;
        long value=this->innerSlotsList[this->size]->getNums();
        this->innerSlotsList.pop_back();
        return value;
    }

    void OperandStack::pushFloat(float value) {
        Slots *slots=new Slots;
        slots->setFloatValue(value);
        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    float OperandStack::popFloat() {
        this->size--;
        float value=this->innerSlotsList[this->size]->getFloatValue();
        this->innerSlotsList.pop_back();

        return value;
    }

    void OperandStack::pushDouble(double value) {
        Slots *slots=new Slots;
        slots->setDoubleValue(value);
        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    double OperandStack::popDouble() {
        this->size--;
        double value=this->innerSlotsList[this->size]->getDoubleValue();
        this->innerSlotsList.pop_back();
        return value;
    }

    void OperandStack::pushRef(Object *refs) {
        Slots *slots=new Slots;
        slots->setRefs(refs);
        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    Object *OperandStack::popObject() {
        this->size--;
        Object* object=this->innerSlotsList[this->size]->getRefs();
        this->innerSlotsList.pop_back();
        return object;
    }

    void OperandStack::pushBoolean(bool booleans) {

    }

    bool OperandStack::popBoolean() {
        return false;
    }

    Slots* OperandStack::popSlots() {
        this->size--;
        Slots* slots=this->innerSlotsList[this->size];
        this->innerSlotsList.pop_back();
        return slots;
    }

    void OperandStack::pushSlots(Slots *slots) {

        this->innerSlotsList.push_back(slots);
        this->size=this->size+1;
    }

    void OperandStack::clear() {
        // 注意：Slots 析构会 delete refs；这里仅逻辑清栈，避免误释放仍被堆/局部变量持有的对象。
        innerSlotsList.clear();
        size = 0;
    }

    Object *OperandStack::getRefFromTop(long index) {
        (void)index;
        if (this->size < 1) {
            return nullptr;
        }
        return this->innerSlotsList[static_cast<size_t>(this->size - 1)]->getRefs();
    }

    Object* OperandStack::peekReceiverForInvoke(int totalInvokeSlots) const {
        if (totalInvokeSlots < 1 || this->size < totalInvokeSlots) {
            return nullptr;
        }
        const int base = this->size - totalInvokeSlots;
        Slots* slot = this->innerSlotsList[static_cast<size_t>(base)];
        if (!slot) {
            return nullptr;
        }
        return slot->getRefs();
    }
};