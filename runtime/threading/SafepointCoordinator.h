//
// Safepoint coordination for mutator threads vs GC / VM operations.
//

#ifndef XJVM_SAFEPOINT_COORDINATOR_H
#define XJVM_SAFEPOINT_COORDINATOR_H

#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace Runtime {
namespace Threading {

/**
 * Coordinates a global safepoint: mutators call pollSafepoint() in their
 * interpreter loop; GC (or VM) calls requestSafepoint(), waitAllAtSafepoint(),
 * then resumeWorld().
 *
 * Thread-safe. Typical use: one registration per OS thread running bytecode.
 */
class SafepointCoordinator {
public:
    static SafepointCoordinator& instance();

    void registerThread();
    void unregisterThread();

    void requestSafepoint();
    /** Mutator: block if a safepoint was requested until resumeWorld(). */
    void pollSafepoint();
    /** Wait until all registered mutators have entered pollSafepoint(), or none registered. */
    void waitAllAtSafepoint();
    void resumeWorld();

    // Test / diagnostics
    int registeredCountForTest() const;
    int arrivedCountForTest() const;

    /** Reset internal state (unit tests only; same-process singleton). */
    void resetForTest();

private:
    SafepointCoordinator() = default;

    mutable std::mutex mtx_;
    std::condition_variable cvMutator_;
    std::condition_variable cvGC_;

    int registered_{0};
    int arrived_{0};
    bool safepointRequested_{false};
};

/** RAII: register on construction, unregister on destruction. */
class MutatorRegistration {
public:
    MutatorRegistration();
    ~MutatorRegistration();
    MutatorRegistration(const MutatorRegistration&) = delete;
    MutatorRegistration& operator=(const MutatorRegistration&) = delete;
};

} // namespace Threading
} // namespace Runtime

#endif
