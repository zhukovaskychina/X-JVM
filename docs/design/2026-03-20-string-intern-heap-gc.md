# 字符串驻留（JString）与 JavaHeap / GC 统一设计

## 背景与问题

- 原先 `JString::getJString(ClassLoader*, str)` 在驻留未命中时用 **`new Object()`**，对象不在 `JavaHeap` 的 `youngList`/`eldenList` 中。
- 主路径启用 `JavaHeap` 的标记-清除后，若 ldc 仍分配堆外对象，则 **GC 无法管理**；若将来把字符串误放入堆，**驻留表中的指针在 sweep 后可能悬空**。

## 目标

1. **带堆执行路径**：新 intern 的 `java/lang/String` 实例经 **`JavaHeap::createJavaObject`** 分配，参与分配计数与 sweep。
2. **无堆路径**（类加载、`prepare` 静态初始化等）：保持 **`new Object()`**，不依赖当前线程堆（与历史行为兼容）。
3. **GC 根**：在 `heap != nullptr` 的收集中，对全局驻留表 **`stringMap` 中每个 `Object*` 做 `markObjectGraph`**，避免驻留字符串被误扫。

## 方案概要

| 组件 | 职责 |
|------|------|
| `JavaThread::javaHeap` | 带堆解释时在 `execByteCode(..., heap)` 内由 `ActiveJavaHeapGuard` 绑定/解绑。 |
| `JString::getJString(..., JavaHeap* javaHeap = nullptr)` | `javaHeap != nullptr` 时用堆分配；否则堆外分配。 |
| `JString::forEachInternedObject(Fn)` | 供 GC 遍历驻留表（模板，避免 `std::function`）。 |
| `GarbageCollector::markFromInternedStrings` | 在 `markFromStaticRoots` 之后、sweep 之前，且仅当 `heap != nullptr` 时调用。 |
| `LDC` / `LDC_W` / `LDC2_W` | 字符串常量从 `javaFrame->getJavaThread()->getJavaHeap()` 传入 `getJString`。 |

## 语义说明

- **混合驻留表**：同一 `stringMap` 中可同时存在堆内与堆外字符串（取决于首次 intern 时的路径）。sweep **只删除堆表中的未标记对象**；堆外对象不会被 sweep，行为与改造前一致。
- **与 JDK 差异**：未实现类卸载与弱引用式 intern；当前驻留表等价于 **强根集合**（与「字符串池常驻」的简化模型一致）。

## 测试与后续

- 单测：`forEachInternedObject` 空操作可安全调用（见 `string_intern_gc_test.cpp`）。
- 后续：在具备最小 `ClassPath` 的集成测试中验证「仅驻留、无栈引用时经 GC 仍存活」。

## 相关文档

- 完整 GC 根集合（栈 / ClassLoader / MethodArea / 驻留表）与 **RuntimeConstantsPool 为何不扫**：见 `2026-03-20-gc-root-sets.md`。
