//
// Created by zhukovasky on 2020/9/8.
//

#include "GarbageCollector.h"
#include "../../Jvm.h"

namespace Runtime{

    GCThreadPool::GCThreadPool():ThreadPool()  {
        this->work= false;
    }

    GCThreadPool::~GCThreadPool() {

    }

    void GCThreadPool::signalWait() {
        this->work= false;
    }

    void GCThreadPool::finalize() {
        ThreadPool::finalize();
    }

    void GCThreadPool::runPendingWork() {
        while (!done){
            std::unique_lock<mutex> lock(this->sleepMtx);
            while (work!= true){
                sleepCnd.wait(lock);
            }
            this->m.lock();

            if(!this->workQueue.empty()){
                auto task=std::move(this->workQueue.front());
                this->workQueue.pop_front();
                this->m.unlock();
                task();
            } else{
                this->m.unlock();
                std::this_thread::yield();
            }

        }
    }

    void GarbageCollector::mark(Object *object) {

    }

    void GarbageCollector::sweep() {

    }

    void GarbageCollector::markAndSweep() {

    }

    void GarbageCollector::notifyGC() {
        this->safePointWaitCnt=0;
        this->overMemoryThreshold=true;
    }

    void GarbageCollector::stopTheWorld() {
        unique_lock<std::mutex> lock(this->safePointWaitMtx);
        safePointWaitCnt++;
        while (safePointWaitCnt!=4){
            safePointWaitCond.wait(lock);
        }
        safePointWaitCond.notify_all();
    }

    GarbageCollector::GarbageCollector() {
        //初始化GC线程
       this->gcThreadPool.start(std::thread::hardware_concurrency());
       this->overMemoryThreshold= false;
    }

    bool GarbageCollector::shouldGC() {
        return this->overMemoryThreshold;
    }
}