//
// JVM 并发地基：对 std::atomic 的薄封装，便于后续替换为平台内建或 64 位扩展。
//

#ifndef XJVM_RUNTIME_CONCURRENT_ATOMIC_OPS_H
#define XJVM_RUNTIME_CONCURRENT_ATOMIC_OPS_H

#include <atomic>
#include <cstdint>

namespace Runtime {
namespace Concurrent {

template <typename T>
using Atomic = std::atomic<T>;

inline bool compareAndSwap32(std::atomic<int32_t>* cell, int32_t expected, int32_t desired) {
    int32_t e = expected;
    return cell->compare_exchange_strong(e, desired, std::memory_order_acq_rel);
}

inline bool compareAndSwap64(std::atomic<int64_t>* cell, int64_t expected, int64_t desired) {
    int64_t e = expected;
    return cell->compare_exchange_strong(e, desired, std::memory_order_acq_rel);
}

inline void fenceAcquire() {
    std::atomic_thread_fence(std::memory_order_acquire);
}

inline void fenceRelease() {
    std::atomic_thread_fence(std::memory_order_release);
}

inline void fenceSeqCst() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

} // namespace Concurrent
} // namespace Runtime

#endif
