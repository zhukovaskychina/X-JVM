#include "SafepointCoordinator.h"

namespace Runtime {
namespace Threading {

SafepointCoordinator& SafepointCoordinator::instance() {
    static SafepointCoordinator inst;
    return inst;
}

void SafepointCoordinator::registerThread() {
    std::lock_guard<std::mutex> lock(mtx_);
    ++registered_;
}

void SafepointCoordinator::unregisterThread() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (registered_ > 0) {
        --registered_;
    }
    cvGC_.notify_all();
}

void SafepointCoordinator::requestSafepoint() {
    std::lock_guard<std::mutex> lock(mtx_);
    safepointRequested_ = true;
    cvGC_.notify_all();
}

void SafepointCoordinator::pollSafepoint() {
    std::unique_lock<std::mutex> lock(mtx_);
    if (!safepointRequested_) {
        return;
    }
    ++arrived_;
    cvGC_.notify_all();
    while (safepointRequested_) {
        cvMutator_.wait(lock);
    }
    --arrived_;
}

void SafepointCoordinator::waitAllAtSafepoint() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (safepointRequested_ && registered_ > 0 && arrived_ < registered_) {
        cvGC_.wait(lock);
    }
}

void SafepointCoordinator::resumeWorld() {
    std::lock_guard<std::mutex> lock(mtx_);
    safepointRequested_ = false;
    cvMutator_.notify_all();
}

int SafepointCoordinator::registeredCountForTest() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return registered_;
}

int SafepointCoordinator::arrivedCountForTest() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return arrived_;
}

void SafepointCoordinator::resetForTest() {
    std::lock_guard<std::mutex> lock(mtx_);
    registered_ = 0;
    arrived_ = 0;
    safepointRequested_ = false;
}

MutatorRegistration::MutatorRegistration() {
    SafepointCoordinator::instance().registerThread();
}

MutatorRegistration::~MutatorRegistration() {
    SafepointCoordinator::instance().unregisterThread();
}

} // namespace Threading
} // namespace Runtime
