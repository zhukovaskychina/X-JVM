# X-JVM 并发与 GC 基础 — 开发实施计划

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在现有解释器与 `GarbageCollector`/`ThreadPool` 骨架上，落地可验证的 **Safepoint 协调**、**GC 线程唤醒协议**、**解释循环轮询点**，并补齐 **MethodDescriptor 参数解析** 与配套测试，为后续 G1/多线程打地基。

**Architecture:** 引入单例或 VM 持有的 `SafepointCoordinator`（注册线程数 + 请求暂停 + 到达等待 + 恢复）；`Interpret::loopJavaFrame` 每 N 条指令或每次循环头调用 `pollSafepoint()`；GC 侧用条件变量驱动 `GCThreadPool`，替换硬编码 `safePointWaitCnt!=4`；`MethodDescriptorParser::parseParamTypes` 复用已有 `parseFieldType()` 循环直至 `)`。

**Tech Stack:** C++14、现有 `xjvm_core`、GoogleTest、CMake/CTest。

---

## Chunk 1: Safepoint 与 GC 线程协议

### Task 1: 新增 `SafepointCoordinator`

**Files:**
- Create: `runtime/threading/SafepointCoordinator.h`
- Create: `runtime/threading/SafepointCoordinator.cpp`
- Modify: `CMakeLists.txt`（将上述文件加入 `RUNTIME_SOURCES` 或新建 `RUNTIME_THREADING_SOURCES` 并入 `XJVM_CORE_SOURCES`）

- [ ] **Step 1: 定义接口与状态**

在头文件中声明（可按需微调命名，但语义需一致）：

- `void registerThread()` / `void unregisterThread()`：解释线程进入/退出循环时调用。
- `void requestSafepoint()`：GC 或 VM 请求全局暂停。
- `void pollSafepoint()`：mutator 在循环中调用；若已请求暂停则阻塞直到 `resumeWorld()`。
- `void arriveAtSafepoint()`：可选，与 `pollSafepoint` 合并实现亦可。
- `void waitAllAtSafepoint()`：GC 线程等待注册线程全部到达暂停点。
- `void resumeWorld()`：清除请求并唤醒所有 mutator。

使用 `std::mutex`、`std::condition_variable`、`std::atomic<bool>`、`std::atomic<int>` 维护：`safepointRequested`、`registeredCount`、`arrivedCount`。

- [ ] **Step 2: 实现最小正确语义**

实现规则：

- `requestSafepoint` 置位并 `notify_all`（或让 `poll` 自旋检查，推荐条件变量避免忙等）。
- `pollSafepoint`：若 `safepointRequested`，在锁内递增 `arrivedCount` 并 `wait` 直到 `!safepointRequested`（注意与 `waitAllAtSafepoint` 的握手顺序，避免丢失唤醒：用两阶段条件变量或 `generation` 计数）。
- `waitAllAtSafepoint`：`registeredCount == arrivedCount` 且 `safepointRequested` 为真时返回。
- `resumeWorld`：清 `safepointRequested`，`arrivedCount=0`，`notify_all`。

- [ ] **Step 3: 单元测试**

- Create: `tests/safepoint_coordinator_test.cpp`
- Modify: `tests/CMakeLists.txt`（加入新 cpp）、`add_test` 已由 `xjvm_unit_tests` 聚合则只需链接进同一可执行文件。

测试场景（GoogleTest）：

1. 单线程：`request` → `poll` 阻塞 → 另一线程 `waitAll`（单线程时 registered==1）→ `resume` → `poll` 返回。
2. 双线程：两个 `std::thread` 均 `register` + 循环 `poll`，主线程 `request` → 断言两者均到达 → `resume`。

Run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure -R safepoint
```

Expected: 全部 PASS。

- [ ] **Step 4: Commit**

```bash
git add runtime/threading/SafepointCoordinator.h runtime/threading/SafepointCoordinator.cpp CMakeLists.txt tests/safepoint_coordinator_test.cpp tests/CMakeLists.txt
git commit -m "feat(runtime): add SafepointCoordinator and tests"
```

---

### Task 2: 改造 `GarbageCollector::stopTheWorld` 与 `GCThreadPool`

**Files:**
- Modify: `runtime/gc/GarbageCollector.h`
- Modify: `runtime/gc/GarbageCollector.cpp`
- Modify: `runtime/gc/GarbageCollector.h` / `Jvm.h`（若需注入 `SafepointCoordinator` 指针，优先前向声明 + 构造函数注入或单例访问，避免循环依赖）

- [ ] **Step 1: 删除硬编码 `safePointWaitCnt!=4`**

改为调用 `SafepointCoordinator::requestSafepoint()` + `waitAllAtSafepoint()`（或等价 API）。GC 开始前必须保证所有已注册 mutator 已停在 `pollSafepoint`。

- [ ] **Step 2: 统一 GC 唤醒路径**

`notifyGC()` 应：设置内存阈值标志 + 通知 `GCThreadPool` 从 `sleepCnd.wait` 醒来；工作线程执行 `stopTheWorld` → `markAndSweep()`（可为空实现先打日志）→ `resumeWorld()`。

审查 `GCThreadPool::runPendingWork()`：确保 `work==true` 时有路径被设置（例如 `notifyGC` 里 `work=true; sleepCnd.notify_all()`），避免永久阻塞。

- [ ] **Step 3: 在 `Interpret::loopJavaFrame` 注册与轮询**

**Files:**
- Modify: `interpret/Interpret.cpp`
- Modify: `interpret/Interpret.h`（若需持有 coordinator 引用）

在 `execByteCode` 进入 `loopJavaFrame` 前 `registerThread()`，在循环结束或异常路径 `unregisterThread()`（可用 RAII guard 类）。

在 `while(true)` 循环顶部调用 `SafepointCoordinator::instance().pollSafepoint()`（或注入对象）。

- [ ] **Step 4: 构建与冒烟**

```bash
cmake --build build -j
./build/jvm --xjre <path> --classpath . --javaclass Hello.class
```

Expected: 行为与改造前一致（无死锁）；若开启测试 GC 路径，进程应能完成或按设计停顿后恢复。

- [ ] **Step 5: Commit**

```bash
git add runtime/gc/GarbageCollector.cpp runtime/gc/GarbageCollector.h interpret/Interpret.cpp interpret/Interpret.h
git commit -m "feat(gc): wire safepoint into GC and interpreter loop"
```

---

## Chunk 2: MethodDescriptor 参数解析

### Task 3: 实现 `parseParamTypes` 与错误处理

**Files:**
- Modify: `runtime/heap/MethodDescriptor.cpp`
- Modify: `runtime/heap/MethodDescriptor.h`（若需返回 `bool`/`std::optional` 或设置 `error` 字段）

- [ ] **Step 1: 写失败用例测试**

- Modify: `tests/method_descriptor_test.cpp`

新增用例（先写测试，期望当前实现 FAIL 或行为错误）：

- `"(ILjava/lang/String;[I)V"` → 参数列表 `I`, `Ljava/lang/String;`, `[I`
- 非法描述符 `"(V)V"` 或缺失 `)` → 应报告错误且不 `exit(1)`

- [ ] **Step 2: 实现 `parseParamTypes`**

在 `(` 与 `)` 之间循环：

- 若读到 `)` 则结束。
- 否则 `unreadU1()` 后调用 `parseFieldType()`，将非空结果 `push_back` 到 `parameterType`。
- 若 `parseFieldType` 返回空且下一字符不是 `)`，视为错误。

移除或替换 `parseObjectType` 中的 `exit(1)`，改为设置错误状态并返回（由 `parse` 统一处理）。

- [ ] **Step 3: 运行测试**

```bash
ctest --test-dir build --output-on-failure -R MethodDescriptor
```

Expected: 全部 PASS。

- [ ] **Step 4: Commit**

```bash
git add runtime/heap/MethodDescriptor.cpp runtime/heap/MethodDescriptor.h tests/method_descriptor_test.cpp
git commit -m "feat(heap): complete MethodDescriptor parameter parsing"
```

---

## Chunk 3: GC 最小标记骨架（可选本迭代完成）

### Task 4: `mark` / `sweep` 空实现 → 可遍历根

**Files:**
- Modify: `runtime/gc/GarbageCollector.cpp`
- Modify: `runtime/heap/JavaHeap.h` / `JavaHeap.cpp`（若需暴露对象迭代或分配表）
- Modify: `runtime/JavaFrame.h` / `OperandStack`（若需从栈扫描引用）

- [ ] **Step 1: 定义“对象可达”最小模型**

例如：堆中对象链表 + `marked` 位；根仅从当前 `JavaThread` 栈与静态区（若有）扫描 `Object*`。

- [ ] **Step 2: 实现 `mark` DFS/BFS**

从根集合标记；若类型系统未统一 `Object*`，可先只对 `JavaHeap` 中已分配对象做 noop 标记并打日志。

- [ ] **Step 3: 实现 `sweep`**

遍历堆，未标记则释放（或仅计数 + log，避免过早破坏现有分配器）。

- [ ] **Step 4: Commit**

```bash
git commit -m "feat(gc): minimal mark-sweep scaffold with safepoint"
```

---

## 执行顺序与依赖

1. Task 1（SafepointCoordinator + 测试）— 无依赖。
2. Task 2 — 依赖 Task 1。
3. Task 3 — 可与 Task 1 并行，但建议 Task 2 之后合并以减少冲突。
4. Task 4 — 依赖 Task 2 的 STW 正确性。

---

## 完成定义（Definition of Done）

- [ ] `ctest --test-dir build --output-on-failure` 全部通过。
- [ ] 无新增 `exit()` 于 `MethodDescriptor` 解析路径。
- [ ] `stopTheWorld` 不再使用固定线程数 `4`。
- [ ] 解释循环在 safepoint 请求下可停可恢复，无死锁（双线程测试 + 手工冒烟）。

---

**Plan complete and saved to `docs/superpowers/plans/2026-03-20-xjvm-concurrency-gc-foundations.md`. Ready to execute?**
