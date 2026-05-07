//
// jdk.internal.misc.Unsafe 堆外内存相关 native（支撑 NIO DirectByteBuffer 等）。
//

#ifndef XJVM_UNSAFE_NATIVE_SUPPORT_H
#define XJVM_UNSAFE_NATIVE_SUPPORT_H

#include "NativeMethodRegistry.h"

namespace Runtime {
class Object;
namespace Native {

void native_Unsafe_registerNatives(const NativeCallContext& ctx);

void native_Unsafe_getInt(const NativeCallContext& ctx);
void native_Unsafe_putInt(const NativeCallContext& ctx);
void native_Unsafe_getIntVolatile(const NativeCallContext& ctx);
void native_Unsafe_putIntVolatile(const NativeCallContext& ctx);

void native_Unsafe_getLong(const NativeCallContext& ctx);
void native_Unsafe_putLong(const NativeCallContext& ctx);
void native_Unsafe_getLongVolatile(const NativeCallContext& ctx);
void native_Unsafe_putLongVolatile(const NativeCallContext& ctx);

void native_Unsafe_getFloat(const NativeCallContext& ctx);
void native_Unsafe_putFloat(const NativeCallContext& ctx);
void native_Unsafe_getFloatVolatile(const NativeCallContext& ctx);
void native_Unsafe_putFloatVolatile(const NativeCallContext& ctx);

void native_Unsafe_getDouble(const NativeCallContext& ctx);
void native_Unsafe_putDouble(const NativeCallContext& ctx);
void native_Unsafe_getDoubleVolatile(const NativeCallContext& ctx);
void native_Unsafe_putDoubleVolatile(const NativeCallContext& ctx);

void native_Unsafe_getReference(const NativeCallContext& ctx);
void native_Unsafe_putReference(const NativeCallContext& ctx);
void native_Unsafe_getReferenceVolatile(const NativeCallContext& ctx);
void native_Unsafe_putReferenceVolatile(const NativeCallContext& ctx);

void native_Unsafe_getBoolean(const NativeCallContext& ctx);
void native_Unsafe_putBoolean(const NativeCallContext& ctx);
void native_Unsafe_getBooleanVolatile(const NativeCallContext& ctx);
void native_Unsafe_putBooleanVolatile(const NativeCallContext& ctx);

void native_Unsafe_getByte(const NativeCallContext& ctx);
void native_Unsafe_putByte(const NativeCallContext& ctx);
void native_Unsafe_getByteVolatile(const NativeCallContext& ctx);
void native_Unsafe_putByteVolatile(const NativeCallContext& ctx);

void native_Unsafe_getShort(const NativeCallContext& ctx);
void native_Unsafe_putShort(const NativeCallContext& ctx);
void native_Unsafe_getShortVolatile(const NativeCallContext& ctx);
void native_Unsafe_putShortVolatile(const NativeCallContext& ctx);

void native_Unsafe_getChar(const NativeCallContext& ctx);
void native_Unsafe_putChar(const NativeCallContext& ctx);
void native_Unsafe_getCharVolatile(const NativeCallContext& ctx);
void native_Unsafe_putCharVolatile(const NativeCallContext& ctx);

void native_Unsafe_allocateMemory0(const NativeCallContext& ctx);
void native_Unsafe_reallocateMemory0(const NativeCallContext& ctx);
void native_Unsafe_freeMemory0(const NativeCallContext& ctx);
void native_Unsafe_setMemory0(const NativeCallContext& ctx);
void native_Unsafe_copyMemory0(const NativeCallContext& ctx);

void native_Unsafe_fullFence(const NativeCallContext& ctx);
void native_Unsafe_loadFence(const NativeCallContext& ctx);
void native_Unsafe_storeFence(const NativeCallContext& ctx);
void native_Unsafe_compareAndSetReference(const NativeCallContext& ctx);
void native_Unsafe_compareAndExchangeReference(const NativeCallContext& ctx);
void native_Unsafe_compareAndSetInt(const NativeCallContext& ctx);
void native_Unsafe_compareAndExchangeInt(const NativeCallContext& ctx);
void native_Unsafe_compareAndSetLong(const NativeCallContext& ctx);
void native_Unsafe_compareAndExchangeLong(const NativeCallContext& ctx);

void native_Unsafe_objectFieldOffset0(const NativeCallContext& ctx);
void native_Unsafe_objectFieldOffset1(const NativeCallContext& ctx);
void native_Unsafe_staticFieldOffset0(const NativeCallContext& ctx);
void native_Unsafe_staticFieldBase0(const NativeCallContext& ctx);
void native_Unsafe_arrayBaseOffset0(const NativeCallContext& ctx);
void native_Unsafe_arrayIndexScale0(const NativeCallContext& ctx);

void native_Unsafe_shouldBeInitialized0(const NativeCallContext& ctx);
void native_Unsafe_ensureClassInitialized0(const NativeCallContext& ctx);
void native_Unsafe_copySwapMemory0(const NativeCallContext& ctx);
void native_Unsafe_writeback0(const NativeCallContext& ctx);
void native_Unsafe_writebackPreSync0(const NativeCallContext& ctx);
void native_Unsafe_writebackPostSync0(const NativeCallContext& ctx);
void native_Unsafe_getLoadAverage0(const NativeCallContext& ctx);

/** 由 java.lang.Thread.currentThread 等设置，供 Unsafe.park 识别当前线程 OOP。 */
void unsafeParkBindCurrentThreadOop(Object* threadOop);

void native_Unsafe_park(const NativeCallContext& ctx);
void native_Unsafe_unpark(const NativeCallContext& ctx);

void native_Unsafe_allocateInstance(const NativeCallContext& ctx);
void native_Unsafe_throwException(const NativeCallContext& ctx);
void native_Unsafe_defineClass0(const NativeCallContext& ctx);
void native_Unsafe_getUncompressedObject(const NativeCallContext& ctx);
void native_Unsafe_invokeCleaner(const NativeCallContext& ctx);

} // namespace Native
} // namespace Runtime

#endif
