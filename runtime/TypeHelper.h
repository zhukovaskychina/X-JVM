//
// JVM 赋值兼容判断（用于 checkcast / instanceof / aastore 等）
//

#ifndef XJVM_TYPE_HELPER_H
#define XJVM_TYPE_HELPER_H

namespace Runtime {

class JavaClass;
class ClassLoader;

/** 若 value 的运行时类型可赋给 target（含数组协变、`Object`/`Serializable`/`Cloneable`）则 true。 */
bool isJvmAssignable(JavaClass* valueClass, JavaClass* targetClass, ClassLoader* loader);

/**
 * 数组类的一级成分类型（`[Ljava/lang/String;` -> `java/lang/String`，`[[I` -> `[I`）。
 * 仅用于引用/嵌套数组；失败返回 nullptr。
 */
JavaClass* arrayComponentJavaClass(JavaClass* arrayClass, ClassLoader* loader);

} // namespace Runtime

#endif
