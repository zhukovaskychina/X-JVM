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

    class JavaHeap;
    class JavaThread;
    class JavaClass;
    class ClassLoader;
    class MethodArea;

    class GCThreadPool:public ThreadPool{
    public:
        GCThreadPool();

        ~GCThreadPool();


        void signalWait();

        void finalize() override;

        void runPendingWork() override;

        /** Wake workers blocked in runPendingWork (GC work flag). */
        void signalWork();

    public:


    private:
        std::mutex sleepMtx;
        std::atomic_bool work{};

        std::condition_variable sleepCnd;
    };


    class GarbageCollector {

    public:
        /** 进程内唯一收集器，避免多处构造导致重复 GC 线程池。 */
        static GarbageCollector& instance();

        void stopTheWorld();

        void notifyGC();

        bool shouldGC();

        /**
         * @param heap 若非空，从线程栈与可选根集追溯并 sweep 不可达堆对象。
         * @param classLoader 若非空，将其 `classMap` 中类的静态 ref 槽作为根。
         * @param methodArea 若非空，将其 `classTable` 中类的静态 ref 槽作为根（与 ClassLoader 独立）。
         */
        void GC(Runtime::JavaFrame *javaFrame, JavaHeap* heap = nullptr, ClassLoader* classLoader = nullptr,
                MethodArea* methodArea = nullptr);

        /** Mark phase helpers (public for functor-based root scanning without std::function). */
        void markObjectGraph(Object* object);
        void markFromFrame(JavaFrame* frame);

    private:
        GarbageCollector();
        GarbageCollector(const GarbageCollector&) = delete;
        GarbageCollector& operator=(const GarbageCollector&) = delete;

        void runMarkAndSweep(JavaThread* thread, JavaHeap* heap, ClassLoader* classLoader, MethodArea* methodArea);

        void markStaticsOfJavaClass(JavaClass* jc);

        void markFromStaticRoots(ClassLoader* loader);

        void markFromMethodAreaStatics(MethodArea* area);

        void markFromInternedStrings();

    private:
        GCThreadPool gcThreadPool;
        std::atomic_bool overMemoryThreshold;
        std::mutex  overMemoryThresholdMtx;
    };

}


#endif //JVM_GARBAGECOLLECTOR_H
