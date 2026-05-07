# GC 根集合（栈 / ClassLoader / MethodArea / 字符串驻留）

## 当前根集合

| 来源 | 说明 |
|------|------|
| 解释器栈 | 各 `JavaFrame` 的操作数栈与局部变量表中的 `ref` 槽。 |
| `ClassLoader::classMap` | 对每个已加载 `JavaClass` 扫描 `getStaticVars()` 中 `type == "ref"` 的槽。 |
| `MethodArea::classTable` | 与 ClassLoader **独立**；经方法区加载的类只出现在此表时，必须单独扫描，否则静态 `ref` 漏标。 |
| `JString` 驻留表 | 带 `JavaHeap` 的收集中，在 sweep 前对 `stringMap` 中对象 `markObjectGraph`（见 `2026-03-20-string-intern-heap-gc.md`）。 |

`GarbageCollector::GC(javaFrame, heap, classLoader, methodArea)` 中后两者均可为 `nullptr`；`main` 当前仅传 `ClassLoader`，`MethodArea` 传 `nullptr`。若未来 `JavaRuntimeEnv` 走方法区加载主类，应对应传入 `methodArea`。

## 运行时常量池（RuntimeConstantsPool）与 GC

- `RuntimeConstantsPool` 持有 `ConstantsPoolObject` 向量：数值、UTF-8 字面量、`ClassRef` / `FieldRef` / `MethodRef` 等 **符号引用元数据**。
- **当前实现不在池中缓存堆上的 `Object*`**（例如 ldc 创建的 `java/lang/String` 只出现在栈/静态槽/驻留表）。
- 因此 **无需** 将 `RuntimeConstantsPool` 整体作为 GC 根；若日后引入「解析后的堆 Class 镜像」等并缓存在池中，再为相应槽位增加标记扫描。
