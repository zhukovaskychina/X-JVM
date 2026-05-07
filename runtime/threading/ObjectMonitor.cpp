#include "ObjectMonitor.h"

#include "../JavaThread.h"
#include "../Object.h"

#include <chrono>
#include <stdexcept>

namespace Runtime {
namespace Threading {

ObjectMonitorTable& ObjectMonitorTable::instance() {
    static ObjectMonitorTable inst;
    return inst;
}

void ObjectMonitorTable::monitorEnter(Object* obj, JavaThread* thread) {
    if (!obj || !thread) {
        throw std::runtime_error("monitorenter: null object/thread");
    }
    std::unique_lock<std::mutex> lock(mtx_);
    bool contended = false;
    for (;;) {
        auto it = monitors_.find(obj);
        if (it == monitors_.end()) {
            MonitorState fresh;
            fresh.owner = thread;
            fresh.recursion = 1;
            monitors_.emplace(obj, fresh);
            break;
        }
        MonitorState& state = it->second;
        if (state.owner == nullptr || state.owner == thread) {
            state.owner = thread;
            ++state.recursion;
            break;
        }
        if (!contended) {
            thread->setThreadState(JavaThreadState::Blocked);
            contended = true;
        }
        cv_.wait(lock);
    }
    if (contended) {
        thread->setThreadState(JavaThreadState::Runnable);
    }
}

void ObjectMonitorTable::monitorExit(Object* obj, JavaThread* thread) {
    if (!obj || !thread) {
        throw std::runtime_error("monitorexit: null object/thread");
    }
    std::unique_lock<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end() || it->second.owner != thread || it->second.recursion <= 0) {
        throw std::runtime_error("monitorexit: IllegalMonitorStateException");
    }
    --it->second.recursion;
    if (it->second.recursion == 0) {
        it->second.owner = nullptr;
        if (it->second.waiters == 0) {
            monitors_.erase(it);
        }
        cv_.notify_all();
    }
}

void ObjectMonitorTable::monitorWait(Object* obj, JavaThread* thread, long millis) {
    if (!obj || !thread) {
        throw std::runtime_error("wait: null object/thread");
    }
    if (millis < 0) {
        throw std::runtime_error("wait: IllegalArgumentException (negative timeout)");
    }
    if (thread->isInterrupted(true)) {
        throw std::runtime_error("wait: InterruptedException");
    }
    std::unique_lock<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end() || it->second.owner != thread || it->second.recursion <= 0) {
        throw std::runtime_error("wait: IllegalMonitorStateException");
    }
    thread->setThreadState(millis > 0 ? JavaThreadState::TimedWaiting : JavaThreadState::Waiting);

    MonitorState& st = it->second;
    const int savedRecursion = st.recursion;
    ++st.waiters;

    st.owner = nullptr;
    st.recursion = 0;
    cv_.notify_all();

    if (millis <= 0) {
        while (st.pendingSignals <= 0) {
            cv_.wait(lock);
        }
    } else {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(millis);
        while (st.pendingSignals <= 0) {
            if (thread->isInterrupted(false)) {
                break;
            }
            const auto now = std::chrono::steady_clock::now();
            if (now >= deadline) {
                break;
            }
            auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now);
            if (remaining > std::chrono::milliseconds(5)) {
                remaining = std::chrono::milliseconds(5);
            }
            cv_.wait_for(lock, remaining);
        }
    }

    if (st.pendingSignals > 0) {
        --st.pendingSignals;
    }

    bool blockedReenter = false;
    while (st.owner != nullptr && st.owner != thread) {
        if (!blockedReenter) {
            thread->setThreadState(JavaThreadState::Blocked);
            blockedReenter = true;
        }
        cv_.wait(lock);
    }
    st.owner = thread;
    st.recursion = savedRecursion;
    --st.waiters;
    thread->setThreadState(JavaThreadState::Runnable);
    if (thread->isInterrupted(true)) {
        throw std::runtime_error("wait: InterruptedException");
    }
}

void ObjectMonitorTable::monitorNotify(Object* obj, JavaThread* thread) {
    if (!obj || !thread) {
        throw std::runtime_error("notify: null object/thread");
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end() || it->second.owner != thread || it->second.recursion <= 0) {
        throw std::runtime_error("notify: IllegalMonitorStateException");
    }
    if (it->second.waiters > 0) {
        ++it->second.pendingSignals;
    }
    cv_.notify_one();
}

void ObjectMonitorTable::monitorNotifyAll(Object* obj, JavaThread* thread) {
    if (!obj || !thread) {
        throw std::runtime_error("notifyAll: null object/thread");
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end() || it->second.owner != thread || it->second.recursion <= 0) {
        throw std::runtime_error("notifyAll: IllegalMonitorStateException");
    }
    if (it->second.waiters > 0) {
        it->second.pendingSignals += it->second.waiters;
    }
    cv_.notify_all();
}

bool ObjectMonitorTable::holdsLock(Object* obj, JavaThread* thread) const {
    if (!obj || !thread) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end()) {
        return false;
    }
    return it->second.owner == thread && it->second.recursion > 0;
}

void ObjectMonitorTable::resetForTest() {
    std::lock_guard<std::mutex> lock(mtx_);
    monitors_.clear();
}

int ObjectMonitorTable::recursionForTest(Object* obj) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = monitors_.find(obj);
    if (it == monitors_.end()) {
        return 0;
    }
    return it->second.recursion;
}

int ObjectMonitorTable::monitorCountForTest() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return static_cast<int>(monitors_.size());
}

} // namespace Threading
} // namespace Runtime
