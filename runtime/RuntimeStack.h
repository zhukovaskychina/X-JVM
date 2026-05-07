//
// Created by zhukovasky on 2020/8/11.
//

#ifndef JAVAP_RUNTIMESTACK_H
#define JAVAP_RUNTIMESTACK_H


#include "JavaFrame.h"
#include <utility>
#include <vector>

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

        /** Visit each frame from bottom to top without permanently altering the stack. */
        template <typename Fn>
        void forEachFrame(Fn&& fn) {
            std::vector<JavaFrame*> tmp;
            while (!this->innerStack.empty()) {
                tmp.push_back(this->innerStack.top());
                this->innerStack.pop();
            }
            for (auto it = tmp.rbegin(); it != tmp.rend(); ++it) {
                this->innerStack.push(*it);
            }
            for (JavaFrame* f : tmp) {
                if (f) {
                    fn(f);
                }
            }
        }


    private:
        std::stack<Runtime::JavaFrame*> innerStack;
        int maxStackSize;
    };
}



#endif //JAVAP_RUNTIMESTACK_H
