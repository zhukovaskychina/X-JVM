//
// Created by zhukovasky on 2020/9/8.
//

#ifndef JVM_GARBAGECOLLECTOR_H
#define JVM_GARBAGECOLLECTOR_H

#include <thread>
#include <mutex>
#include "../ThreadPool.h"
#include "../Object.h"
#include "../JavaFrame.h"

namespace Runtime{

    class GCThreadPool:public ThreadPool{
    public:
        GCThreadPool();

        ~GCThreadPool();


        void signalWait();

        void finalize() override;

        void runPendingWork() override;

    public:


    private:
        std::mutex sleepMtx;
        std::atomic_bool work{};

        std::condition_variable sleepCnd;
    };


    class GarbageCollector {

    public:
        GarbageCollector();

        void stopTheWorld();

        void notifyGC();

        bool shouldGC();

        void GC(Runtime::JavaFrame *javaFrame);

    private:

        void mark(Object* object);

        void sweep();

        void markAndSweep();


    private:
        GCThreadPool gcThreadPool;
        std::atomic_bool overMemoryThreshold;
        std::mutex  overMemoryThresholdMtx;
        std::mutex  safePointWaitMtx;
        int safePointWaitCnt;
        std::condition_variable safePointWaitCond;
    };

}


#endif //JVM_GARBAGECOLLECTOR_H
