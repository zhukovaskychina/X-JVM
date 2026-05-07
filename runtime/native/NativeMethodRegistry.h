//
// X-JVM：native 方法注册表（类名 + 方法名 + 描述符 -> C++ 实现）
//

#ifndef XJVM_NATIVE_METHOD_REGISTRY_H
#define XJVM_NATIVE_METHOD_REGISTRY_H

#include <mutex>
#include <string>
#include <unordered_map>

namespace Runtime {

class JavaFrame;
class JavaClass;
class ClassLoader;
class Object;

namespace Heap {
class Method;
}

namespace Native {

struct NativeCallContext {
    JavaFrame* caller;
    JavaFrame* callee;
    Heap::Method* method;
};

using NativeMethodHandler = void (*)(const NativeCallContext& ctx);

class NativeMethodRegistry {
public:
    static NativeMethodRegistry& instance();

    bool registerNative(const std::string& classInternalName,
                        const std::string& methodName,
                        const std::string& descriptor,
                        NativeMethodHandler handler);

    bool tryInvoke(JavaFrame* caller, JavaFrame* callee, Heap::Method* method);

    void resetForTesting();

    /** 在持锁下合并内置桩（resetForTesting 后调用以恢复）。 */
    void installBuiltInNatives();

private:
    NativeMethodRegistry();
    NativeMethodRegistry(const NativeMethodRegistry&) = delete;
    NativeMethodRegistry& operator=(const NativeMethodRegistry&) = delete;

    void registerBuiltInsUnlocked();
    void registerNativeUnlocked(const std::string& classInternalName,
                                const std::string& methodName,
                                const std::string& descriptor,
                                NativeMethodHandler handler);

    static std::string makeKey(const std::string& cls, const std::string& name, const std::string& desc);

    std::mutex mu_;
    std::unordered_map<std::string, NativeMethodHandler> handlers_;
};

void registerBuiltInNatives();

/** 确保主线程 Thread 桩存在并将 TLS 绑定到 Unsafe.park（无需先调用 Thread.currentThread）。 */
void ensureParkTlsBoundForLoader(ClassLoader* cl);

/** Class 镜像 <-> JavaClass*（供 Unsafe / 其他 native 复用）。 */
JavaClass* javaClassFromClassMirrorObject(Object* mirror);
Object* mirrorForJavaClass(JavaClass* jc, ClassLoader* cl);

} // namespace Native
} // namespace Runtime

#endif
