# X-JVM 单元测试

- **框架**: GoogleTest（源码位于 `third_party/googletest/`，由顶层 `CMakeLists.txt` 引入）
- **可执行文件**: `xjvm_unit_tests`（链接静态库 `xjvm_core`，与 `jvm` 主程序共享同一套核心源码）

## 构建与运行

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

仅构建测试目标：

```bash
cmake --build build --target xjvm_unit_tests
./build/tests/xjvm_unit_tests   # 路径因生成器/平台可能略有不同
```

关闭测试（加快配置/默认构建）：

```bash
cmake -S . -B build -DXJVM_BUILD_TESTS=OFF
```

## 现有用例

| 文件 | 覆盖范围 |
|------|----------|
| `string_utils_test.cpp` | `Utils::StringUtils` 常用字符串工具 |
| `byte_queue_test.cpp` | `Lang::ByteQueue` 大端数值与 magic |
| `method_descriptor_test.cpp` | `MethodDescriptorParser`（含带参/非法描述符） |
| `platform_compat_test.cpp` | `PlatformCompat::split` / `readFileContent` / `safeFmod` |
| `safepoint_coordinator_test.cpp` | `SafepointCoordinator` / `MutatorRegistration` |
| `gc_heap_test.cpp` | `JavaHeap` 标记清扫、分配阈值 `shouldCollect` |
| `string_intern_gc_test.cpp` | `JString` 驻留表遍历（与堆/GC 设计见 `docs/design/2026-03-20-string-intern-heap-gc.md`） |
| `native_registry_test.cpp` | `NativeMethodRegistry` 注册/去重与内置桩恢复（见 `docs/design/2026-03-21-native-method-registry.md`） |

主程序 `main.cpp` 已使用 `JavaHeap` + `execByteCode(..., &heap, classLoader)`（可选第四参 `MethodArea*`，见 `docs/design/2026-03-20-gc-root-sets.md`）。GC 在带堆路径下扫描线程栈；在传入 `ClassLoader` / `MethodArea` 时分别扫描对应表中已加载类的静态 `ref` 槽；带堆收集时还会扫描 **字符串驻留表**。

后续可优先补充：`ClassFile` 最小合法 class 字节、与 `GarbageCollector::instance()` 联调的集成测试（含静态字段可达性）。
