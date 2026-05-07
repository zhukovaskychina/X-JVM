# GC Phase 4–5 落地说明（简记）

当前主线为 **标记-清扫**（`GarbageCollector` + `JavaHeap::shouldCollect`），根集合覆盖线程栈、静态字段、字符串驻留等（见 `tests/README.md`）。

向 **G1 风格分区化** 演进时的建议顺序（与 `docs/PROJECT_IMPROVEMENT_PLAN.md` Phase 4 一致）：

1. **Region 元数据**：对象归属 region id、eden/survivor/old 逻辑标签。
2. **卡表 + 写屏障**：记录跨 region / 跨代引用，为增量/并发标记做准备。
3. **年轻代复制**：从「全堆清扫」中拆出可频繁触发的小收集。
4. **暂停目标统计**：先记录 pause wall time，再引入启发式选 region。

ZGC 预研（Phase 5）应在 G1 路径有稳定回归集后再并行开启，保持 `IGarbageCollector` 级抽象便于双路切换。
