//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_RUNTIMESTACK_H
#define JAVAP_RUNTIMESTACK_H


#include "JavaFrame.h"

namespace Runtime{
    class JavaFrame;
    class RuntimeStack {
    public:
        RuntimeStack();
        RuntimeStack(int maxStackSize);

        virtual ~RuntimeStack();
        void push(Runtime::JavaFrame* javaFrame);

        Runtime::JavaFrame* pop();

        Runtime::JavaFrame* top();

        bool isEmpty();

        int getStackSize();

        void clear();


    private:
        std::stack<Runtime::JavaFrame*> innerStack;
        int maxStackSize;
    };
}



#endif //JAVAP_RUNTIMESTACK_H
