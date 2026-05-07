//
// 解释器内抛出的 Java 异常对象（由 athrow 等设置，由 Interpret 捕获并分派）。
//

#ifndef XJVM_JVM_THROWN_OBJECT_H
#define XJVM_JVM_THROWN_OBJECT_H

#include <exception>

namespace Runtime {

class Object;

class JvmThrownObject : public std::exception {
public:
    explicit JvmThrownObject(Object *thrown) : thrown_(thrown) {}

    Object *thrownObject() const { return thrown_; }

    const char *what() const noexcept override { return "JvmThrownObject"; }

private:
    Object *thrown_{nullptr};
};

} // namespace Runtime

#endif
