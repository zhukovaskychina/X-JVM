//
// Created by zhukovasky on 2020/9/8.
//

#include "GarbageCollector.h"
#include "../threading/SafepointCoordinator.h"
#include "../JavaThread.h"
#include "../ClassLoader.h"
#include "../MethodArea.h"
#include "../heap/JavaHeap.h"
#include "../heap/JavaClass.h"
#include "../heap/StringConstantPools.h"
#include "../Slots.h"

namespace Runtime{

    namespace {

    struct MarkObjectFromOperandStack {
        GarbageCollector* gc;
        void operator()(Object* r) const {
            if (gc && r) {
                gc->markObjectGraph(r);
            }
        }
    };

    struct MarkFromStackFrame {
        GarbageCollector* gc;
        void operator()(JavaFrame* f) const {
            if (gc && f) {
                gc->markFromFrame(f);
            }
        }
    };

    } // namespace

    GarbageCollector& GarbageCollector::instance() {
        static GarbageCollector inst;
        return inst;
    }

    GCThreadPool::GCThreadPool():ThreadPool()  {
        this->work= false;
    }

    GCThreadPool::~GCThreadPool() {

    }

    void GCThreadPool::signalWait() {
        this->work= false;
    }

    void GCThreadPool::signalWork() {
        std::lock_guard<std::mutex> lock(this->sleepMtx);
        this->work = true;
        this->sleepCnd.notify_all();
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
                if (task) {
                    task->run();
                }
            } else{
                this->m.unlock();
                std::this_thread::yield();
            }

        }
    }

    void GarbageCollector::markObjectGraph(Object *object) {
        if (!object || object->isGcMarked()) {
            return;
        }
        object->setGcMarked(true);
        JavaClass* cls = object->getJavaClass();
        if (!cls) {
            return;
        }
        const int alen = object->getArrayLength();
        if (alen > 0 && !cls->isPrimitiveArray()) {
            Object** refs = reinterpret_cast<Object**>(object->getData());
            if (refs) {
                for (int i = 0; i < alen; ++i) {
                    if (refs[i]) {
                        markObjectGraph(refs[i]);
                    }
                }
            }
        }
        Slots* base = object->getFields();
        if (!base) {
            return;
        }
        const int n = static_cast<int>(cls->getInstanceCount());
        for (int i = 0; i < n; ++i) {
            Slots& slot = base[i];
            if (slot.getSlotType() == "ref") {
                Object* r = slot.getRefs();
                if (r) {
                    markObjectGraph(r);
                }
            }
        }
    }

    void GarbageCollector::markFromFrame(JavaFrame *frame) {
        if (!frame) {
            return;
        }
        MarkObjectFromOperandStack markRef{this};
        OperandStack* os = frame->getOperandStack();
        if (os) {
            os->visitObjectRefs(markRef);
        }
        LocalVariableTables* lv = frame->getLocalVariableTables();
        if (lv) {
            lv->visitObjectRefs(markRef);
        }
    }

    void GarbageCollector::markStaticsOfJavaClass(JavaClass *jc) {
        if (!jc) {
            return;
        }
        for (Slots* s : jc->getStaticVars()) {
            if (!s) {
                continue;
            }
            if (s->getSlotType() == "ref") {
                Object* r = s->getRefs();
                if (r) {
                    markObjectGraph(r);
                }
            }
        }
    }

    void GarbageCollector::markFromStaticRoots(ClassLoader *loader) {
        if (!loader) {
            return;
        }
        loader->forEachLoadedClass([this](JavaClass* jc) {
            markStaticsOfJavaClass(jc);
        });
    }

    void GarbageCollector::markFromMethodAreaStatics(MethodArea *area) {
        if (!area) {
            return;
        }
        area->forEachLoadedClass([this](JavaClass* jc) {
            markStaticsOfJavaClass(jc);
        });
    }

    void GarbageCollector::markFromInternedStrings() {
        Heap::JString::getJString()->forEachInternedObject([this](Object* o) {
            if (o) {
                markObjectGraph(o);
            }
        });
    }

    void GarbageCollector::runMarkAndSweep(JavaThread *thread, JavaHeap *heap, ClassLoader *classLoader,
                                           MethodArea *methodArea) {
        if (heap) {
            heap->clearGcMarksOnAllObjects();
        }
        if (thread && thread->getJavaStack()) {
            MarkFromStackFrame walkFrames{this};
            thread->getJavaStack()->forEachFrame(walkFrames);
        }
        markFromStaticRoots(classLoader);
        markFromMethodAreaStatics(methodArea);
        if (heap) {
            markFromInternedStrings();
        }
        if (heap) {
            heap->sweepUnmarkedObjects();
        }
    }

    void GarbageCollector::notifyGC() {
        this->overMemoryThreshold.store(true);
        this->gcThreadPool.signalWork();
    }

    void GarbageCollector::stopTheWorld() {
        auto& sp = Threading::SafepointCoordinator::instance();
        sp.requestSafepoint();
        sp.waitAllAtSafepoint();
    }

    void GarbageCollector::GC(Runtime::JavaFrame *javaFrame, JavaHeap *heap, ClassLoader *classLoader,
                            MethodArea *methodArea) {
        if (!javaFrame) {
            return;
        }
        stopTheWorld();
        runMarkAndSweep(javaFrame->getJavaThread(), heap, classLoader, methodArea);
        Threading::SafepointCoordinator::instance().resumeWorld();
        this->overMemoryThreshold.store(false);
        if (heap) {
            heap->onGcCompleted();
        }
    }

    GarbageCollector::GarbageCollector() {
        //初始化GC线程
       this->gcThreadPool.start(std::thread::hardware_concurrency());
       this->overMemoryThreshold.store(false);
    }

    bool GarbageCollector::shouldGC() {
        return this->overMemoryThreshold;
    }
}
