#include <gtest/gtest.h>

#include "runtime/heap/StringConstantPools.h"

// 驻留表遍历与单例初始化不应抛异常（完整「堆上 intern + GC 保活」需 JRE 集成测）。
TEST(JStringIntern, ForEachInternedObjectNoThrow) {
    Runtime::Heap::JString::getJString()->forEachInternedObject([](Runtime::Object*) {});
    SUCCEED();
}
