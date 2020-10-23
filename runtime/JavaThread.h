//
// Created by zhukovasky on 2020/8/12.
//

#ifndef JAVAP_JAVATHREAD_H
#define JAVAP_JAVATHREAD_H


#include "RuntimeStack.h"
#include "JavaRuntime.h"
#include <thread>
#include <mutex>
namespace Runtime{
    class JavaFrame;
    class RuntimeStack;
    class JavaHeap;
    class JavaThread {

    public:
        JavaThread();

        static Runtime::JavaThread* newThread(int stackSize);

        //java线程压入栈
        void pushJavaFrame(JavaFrame *javaFrame);

        //java线程出栈
        JavaFrame* popJavaFrame();


        //返回当前栈帧
        JavaFrame* currentFrame();

        int getStackSize() const;

        void setStackSize(int stackSize);

        long getPcAddress() const;

        void setPcAddress(long pcAddress);

        RuntimeStack *getJavaStack() const;

        void setJavaStack(RuntimeStack *javaStack);

    //    const Object &getJThread() const;

    //    void setJThread(const Object &jThread);

        JavaFrame *getJavaFrame() const;

        void setJavaFrame(JavaFrame *javaFrame);

        virtual ~JavaThread();

    private:
        int stackSize;
        long pcAddress;

    //    std::lock_guard<std::mutex> locker;


        std::mutex threadLockerMutex;

        Runtime::RuntimeStack *javaStack;
      //  Runtime::Object jThread;
        Runtime::JavaFrame *javaFrame;

        Runtime::JavaHeap *javaHeap;

        Runtime::JavaRuntimeEnv *jEnv;
    };

}


#endif //JAVAP_JAVATHREAD_H
