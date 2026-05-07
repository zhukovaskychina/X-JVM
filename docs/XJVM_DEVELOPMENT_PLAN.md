# X-JVM 完整开发计划（执行层）

**战略层**仍以 [PROJECT_IMPROVEMENT_PLAN.md](PROJECT_IMPROVEMENT_PLAN.md)（Phase 0–10、JDK17 对标）为准；本文档是**缺口清单 + 近期可执行里程碑**，与仓库实现同步维护。

## 1. 现状摘要

- 核心模块：`classfile/`、`runtime/`、`instructions/`、`interpret/`。
- 已具备：类解析与加载、解释器主循环、**扩展后的字节码子集**（含 `invokespecial` / `invokeinterface` / `getfield` / `putfield` / `wide` / 条件分支等）、异常表分派、标记-清扫 GC、Safepoint、Native 注册表。
- 未知 opcode：`InstructionFactory` 返回 `nullptr`，由解释器抛出 `std::runtime_error`（不再 `exit(1)`）。

## 2. 能力矩阵（简）

| 领域 | 状态 |
|------|------|
| 字节码全集 | 部分（持续补 opcode） |
| Verifier | 不支持 |
| invokedynamic | 不支持 |
| 监视器语义 | 部分（栈桩，见 `Monitor.cpp`） |
| 分代/G1 | 计划中 |
| JVMTI / JIT | 计划中 |

详见 [compatibility/JDK17_COMPAT_MATRIX.md](compatibility/JDK17_COMPAT_MATRIX.md)。

## 3. 优先级（P0–P3）

**P0**：指令覆盖与调用语义（含 `invokespecial`、接口调用、实例字段）、错误 opcode 可诊断。  
**P1**：真实 monitor、线程状态与调度、标准运行时异常路径。  
**P2**：Region/卡表/写屏障、性能基线。  
**P3**：JVMTI 子集、诊断工具、兼容矩阵持续更新。

## 4. 阶段划分

- **Phase A（字节码）**：与战略文档并行；验收为 `javac` 产出用例 + `ctest` 全绿。
- **Phase 0–6**：按 `PROJECT_IMPROVEMENT_PLAN.md` 执行。
- **Phase 7–10**：兼容矩阵、JIT 预研、JMM 子集、运维诊断。

## 5. GC 演进备忘

见 [design/gc-phase4-notes.md](design/gc-phase4-notes.md)。

## 6. 近期周节奏（示例）

1. opcode 审计与文档化已实现表。  
2. `javac` 构造器 / 接口用例各 1 个。  
3. Verifier 最小子集 POC。  
4. 更新兼容矩阵与本文档「现状摘要」。

---

*文档版本：2026-04-25*
