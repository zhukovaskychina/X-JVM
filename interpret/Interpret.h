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

        /**
         * 执行字节码方法
         * @param method 要执行的方法
         * @throws std::runtime_error 当方法为空或执行失败时
         */
        void execByteCode(Runtime::Heap::Method *method);


        /**
         * 执行字节码方法（带堆管理）
         * @param method 要执行的方法
         * @param javaHeap Java堆对象
         * @throws std::runtime_error 当参数为空或执行失败时
         */
        void execByteCode(Runtime::Heap::Method* method,Runtime::JavaHeap* javaHeap);

    private:
        /**
         * 循环执行Java帧中的指令
         * @param javaThread Java线程对象
         * @throws std::runtime_error 当线程为空或执行失败时
         */
        void loopJavaFrame(Runtime::JavaThread* javaThread);

        /**
         * 循环执行Java帧中的指令（带堆管理）
         * @param javaThread Java线程对象
         * @param javaHeap Java堆对象
         * @throws std::runtime_error 当参数为空或执行失败时
         */
        void loopJavaFrameWithHeap(Runtime::JavaThread* javaThread, Runtime::JavaHeap* javaHeap);

    };
}




#endif //JVM_INTERPRET_H
