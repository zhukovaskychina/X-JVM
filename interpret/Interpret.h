//
// Created by zhukovasky on 2020/8/26.
//

#ifndef JVM_INTERPRET_H
#define JVM_INTERPRET_H


#include "../runtime/heap/ClassMember.h"
#include "../runtime/JavaThread.h"
#include "../runtime/heap/JavaHeap.h"

namespace Interpret{
    class Interpret {


    public:
        Interpret();

        virtual ~Interpret();

        void execByteCode(Runtime::Heap::Method *method);


        void execByteCode(Runtime::Heap::Method* method,Runtime::JavaHeap* javaHeap);

    private:
        void loopJavaFrame(Runtime::JavaThread* javaThread);

    };
}




#endif //JVM_INTERPRET_H
