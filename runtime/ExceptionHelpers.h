//
// 构造常见 Java 异常实例并抛出（与 JvmThrownObject / 异常分派配合）。
//

#ifndef XJVM_EXCEPTION_HELPERS_H
#define XJVM_EXCEPTION_HELPERS_H

namespace Runtime {

class JavaFrame;
class Object;

/** 尝试按当前帧类加载器加载 `java/lang/NullPointerException` 并创建实例；失败返回 nullptr。 */
Object *newNullPointerExceptionInstance(JavaFrame *frame);

/**
 * 抛出 NPE：成功则 `throw JvmThrownObject`；类加载失败时退化为 `std::runtime_error`（无堆/无 rt.jar 场景）。
 */
void throwNullPointerException(JavaFrame *frame);

Object *newInstantiationExceptionInstance(JavaFrame *frame);
void throwInstantiationException(JavaFrame *frame);

} // namespace Runtime

#endif
