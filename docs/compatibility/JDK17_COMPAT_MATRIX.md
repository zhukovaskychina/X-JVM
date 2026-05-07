# JDK 17 兼容矩阵（骨架）

本表与 CI / 手工运行结果同步更新。状态：`支持` | `部分` | `不支持` | `计划中`。

| 维度 | 项 | 状态 | 备注 |
|------|----|------|------|
| ClassFile | 解析常见版本 | 部分 | 以当前 `classfile/` 解析能力为准 |
| 字节码 | invokespecial / invokeinterface / getfield / putfield | 部分 | 见 `InstructionFactory.cpp` |
| 字节码 | invokedynamic、完整 switch/wide 子集 | 不支持 | 计划中 |
| 异常 | exception_table 分派与栈展开 | 支持 | `ExceptionDispatch.cpp` |
| GC | 标记-清扫 + 根集合 | 部分 | 见 `GarbageCollector.cpp`、`tests/gc_heap_test.cpp` |
| GC | G1 / 写屏障 / Region | 计划中 | 见 `PROJECT_IMPROVEMENT_PLAN.md` Phase 4 |
| 并发 | monitorenter/exit 语义 | 部分 | 已有最小监视器（重入+所有权校验），尚未实现 wait/notify 与膨胀锁 |
| 并发 | 锁膨胀、wait/notify | 不支持 | Phase 2 |
| Native | 注册表与桩 | 部分 | `NativeMethodRegistry.cpp` |
| JVMTI | — | 不支持 | Phase 6 |

## 更新记录

- 2026-04-25：创建骨架，与执行层开发计划对齐。
