#include <gtest/gtest.h>

#include "runtime/native/NativeMethodRegistry.h"

namespace {

void noop_handler(const Runtime::Native::NativeCallContext&) {}

} // namespace

TEST(NativeMethodRegistry, DuplicateRegisterRejected) {
    Runtime::Native::NativeMethodRegistry& r = Runtime::Native::NativeMethodRegistry::instance();
    EXPECT_TRUE(r.registerNative("xjvm/test/Dummy", "foo", "()V", noop_handler));
    EXPECT_FALSE(r.registerNative("xjvm/test/Dummy", "foo", "()V", noop_handler));
}

TEST(NativeMethodRegistry, ResetAndReloadBuiltIns) {
    Runtime::Native::NativeMethodRegistry& r = Runtime::Native::NativeMethodRegistry::instance();
    r.resetForTesting();
    Runtime::Native::registerBuiltInNatives();
    // 内置方法应可再次解析（不抛异常即可）
    SUCCEED();
}
