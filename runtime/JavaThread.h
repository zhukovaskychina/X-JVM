//
// Created by zhukovasky on 2020/8/12.
//

#ifndef JAVAP_JAVATHREAD_H
#define JAVAP_JAVATHREAD_H


#include "RuntimeStack.h"
#include "JavaRuntime.h"
#include <atomic>
#include <cstdint>
#include <thread>
#include <mutex>
namespace Runtime{
    class JavaFrame;
    class RuntimeStack;
    class JavaHeap;

    /** 解释器线程粗粒度状态（与 java.lang.Thread.State 对齐方向；后续接 park/safepoint）。 */
    enum class JavaThreadState : std::uint8_t {
        New = 0,
        Runnable,
        Blocked,
        Waiting,
        TimedWaiting,
        Terminated
    };

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

        /** 当前解释线程关联的堆（带堆执行路径设置；ldc 等据此把驻留字符串分配进堆）。 */
        JavaHeap *getJavaHeap() const;

        void setJavaHeap(JavaHeap *heap);

        JavaThreadState getThreadState() const { return threadState_; }
        void setThreadState(JavaThreadState s) { threadState_ = s; }

        void interrupt();
        bool isInterrupted(bool clear);

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

        JavaThreadState threadState_{JavaThreadState::New};
        std::atomic<bool> interrupted_{false};
    };

}


#endif //JAVAP_JAVATHREAD_H
