//
// Created by zhukovasky on 2020/8/19.
//

#include "JavaHeap.h"
#include "../Object.h"
#include <vector>

namespace Runtime{

    void JavaHeap::recordAllocation() {
        ++allocationsSinceLastGc_;
    }

    std::size_t JavaHeap::allocationsSinceLastGc() const {
        return allocationsSinceLastGc_;
    }

    std::size_t JavaHeap::gcAllocationThreshold() const {
        return gcAllocationThreshold_;
    }

    void JavaHeap::setGcAllocationThreshold(std::size_t n) {
        gcAllocationThreshold_ = n;
    }

    bool JavaHeap::shouldCollect() const {
        return gcAllocationThreshold_ > 0 && allocationsSinceLastGc_ >= gcAllocationThreshold_;
    }

    void JavaHeap::onGcCompleted() {
        allocationsSinceLastGc_ = 0;
    }

    Object *JavaHeap::createJavaObject(JavaClass *javaClass) {
        Object *object=new Object();
        object->setJavaClass(javaClass);
        object->setData(nullptr);
        this->youngList.insert(object);
        recordAllocation();
        return object;
    }

    Object *JavaHeap::createJavaArrayObject(JavaClass *javaClass) {
        Object *object=new Object();
        object->setJavaClass(javaClass);
        object->setData(nullptr);
        this->eldenList.insert(object);
        recordAllocation();
        return object;
    }

    void JavaHeap::clearGcMarksOnAllObjects() {
        for (Object* o : youngList) {
            if (o) {
                o->clearGcMark();
            }
        }
        for (Object* o : eldenList) {
            if (o) {
                o->clearGcMark();
            }
        }
    }

    static void sweepSet(std::set<Object*>& s) {
        std::vector<Object*> dead;
        for (Object* o : s) {
            if (o && !o->isGcMarked()) {
                dead.push_back(o);
            }
        }
        for (Object* o : dead) {
            s.erase(o);
            Object::deleteInstance(o);
        }
        for (Object* o : s) {
            if (o) {
                o->clearGcMark();
            }
        }
    }

    void JavaHeap::sweepUnmarkedObjects() {
        sweepSet(youngList);
        sweepSet(eldenList);
    }

    std::size_t JavaHeap::totalTrackedObjects() const {
        return youngList.size() + eldenList.size();
    }
}
