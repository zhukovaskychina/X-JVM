//
// Minimal JVM monitor table for monitorenter/monitorexit.
//

#ifndef XJVM_OBJECT_MONITOR_H
#define XJVM_OBJECT_MONITOR_H

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace Runtime {
class Object;
class JavaThread;

namespace Threading {

class ObjectMonitorTable {
public:
    static ObjectMonitorTable& instance();

    void monitorEnter(Object* obj, JavaThread* thread);
    void monitorExit(Object* obj, JavaThread* thread);
    void monitorWait(Object* obj, JavaThread* thread, long millis);
    void monitorNotify(Object* obj, JavaThread* thread);
    void monitorNotifyAll(Object* obj, JavaThread* thread);
    bool holdsLock(Object* obj, JavaThread* thread) const;

    // test helpers
    void resetForTest();
    int recursionForTest(Object* obj) const;
    int monitorCountForTest() const;

private:
    struct MonitorState {
        JavaThread* owner{nullptr};
        int recursion{0};
        int waiters{0};
        int pendingSignals{0};
    };

    ObjectMonitorTable() = default;

    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::unordered_map<Object*, MonitorState> monitors_;
};

} // namespace Threading
} // namespace Runtime

#endif
