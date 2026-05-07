//
// 数组 Object 与元素 payload 单次 malloc + placement new，使
// (payload - object*) 为常量，满足 Unsafe.arrayBaseOffset 与 CAS 字节偏移语义。
//

#ifndef XJVM_ARRAY_COALLOC_LAYOUT_H
#define XJVM_ARRAY_COALLOC_LAYOUT_H

#include "../Object.h"

#include <cstddef>
#include <cstdint>

namespace Runtime {
namespace ArrayCoalloc {

constexpr uint64_t kMagic = 0xA1110C0CA1110C0CULL;

inline std::size_t objectOffset() {
    return (sizeof(uint64_t) + alignof(Object) - 1u) & ~(alignof(Object) - 1u);
}

inline std::size_t payloadOffset() {
    const std::size_t o = objectOffset();
    return (o + sizeof(Object) + alignof(std::max_align_t) - 1u) & ~(alignof(std::max_align_t) - 1u);
}

/** 与 OpenJDK 「oop + base」一致：base = 首元素地址 - oop。 */
inline int baseOffsetBytes() {
    return static_cast<int>(payloadOffset() - objectOffset());
}

} // namespace ArrayCoalloc
} // namespace Runtime

#endif
