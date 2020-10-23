//
// Created by zhukovasky on 2020/8/11.
//

#include "RuntimeStack.h"

namespace Runtime{
    RuntimeStack::RuntimeStack() {

    }

    RuntimeStack::~RuntimeStack() {

    }

    RuntimeStack::RuntimeStack(int maxStackSize) {
        this->maxStackSize=maxStackSize;
    }

    void RuntimeStack::push(Runtime::JavaFrame *javaFrame) {
        if(this->maxStackSize<this->innerStack.size()){
            //报错
        }
        this->innerStack.push(javaFrame);
    }
    //弹出
    Runtime::JavaFrame* RuntimeStack::pop() {
        JavaFrame *javaFrame= this->innerStack.top();
        this->innerStack.pop();

        return javaFrame;
    }

    //返回栈顶
    Runtime::JavaFrame *RuntimeStack::top() {
        JavaFrame *javaFrame= this->innerStack.top();
        return javaFrame;
    }

    bool RuntimeStack::isEmpty() {
        return this->innerStack.empty();
    }

    int RuntimeStack::getStackSize() {
        return this->innerStack.size();
    }

    void RuntimeStack::clear() {
        while (!this->innerStack.empty()){
            delete this->innerStack.top();
        }
    }
}