# Native 方法注册表

## 定位

- **键**：内部类名（`java/lang/System`）+ 方法名 + JNI 描述符（如 `()J`、`(Ljava/lang/Object;ILjava/lang/Object;II)V`）。
- **值**：函数指针 `void (*)(const NativeCallContext&)`；在 `callee` 的局部变量表中读取参数，向 `**caller` 的操作数栈** 压入返回值（`void` 不压栈）。

## API


| 符号                                    | 说明                                          |
| ------------------------------------- | ------------------------------------------- |
| `NativeMethodRegistry::instance()`    | 单例；构造时自动 `registerBuiltInsUnlocked()`。      |
| `registerNative(cls, name, desc, fn)` | 注册自定义 native；已存在则返回 false。                  |
| `tryInvoke(caller, callee, method)`   | `method->isNative()` 且表中有项则调用并返回 true。      |
| `registerBuiltInNatives()`            | `resetForTesting()` 后用于恢复内置桩。               |
| `installBuiltInNatives()`             | 持锁合并内置项（与 `registerBuiltInNatives()` 等价入口）。 |


## 解释器接入

`Index16Instruction::invokeMethod` 在参数拷入 `callee` 局部变量表后，若 `isNative()`，则调用 `tryInvoke`；无论成功与否均 `**popJavaFrame()`**（与 `return` 指令行为一致，不进入字节码循环）。

### 数组（与 `arraycopy` 配套）

- `**newarray`**：一维原始数组，backing 为连续 `uint8_t`（按元素宽度）。
- `**anewarray`（0xbd）**：成分为类或数组成分的引用数组；槽宽 `sizeof(Object*)`，`JavaClass::createArray` 分配 `Object*[]`。
- `**aaload`（0x32） / `aastore`（0x53）**：读写引用槽；越界或异常栈时 `aaload` 压 `null`；`aastore` 对非 null 元素做 `isJvmAssignable` 与数组成分类型 检查，不匹配则**不写槽并静默返回**（未抛 `ArrayStoreException`）。
- `**checkcast`（0xc0） / `instanceof`（0xc1）**：常量池 `Class`；`checkcast` 失败抛 `**std::runtime_error("ClassCastException")`**（由解释器捕获）；`instanceof` 与 `**Runtime::isJvmAssignable**`（含数组协变、数组对 `Object`/`Serializable`/`Cloneable`、接口实现走 `getInterfaces` + 超类链）。
- `**multianewarray`（0xc5）**：`u2` 常量池数组类、`u1` 维数 `d`；从栈顶起依次弹出 最内维→最外维 的长度。`d` 等于类 arity 时递归填满各维；`d` 小于 arity 时仅分配外层，更深维 槽为 null。负长度抛 `**std::runtime_error("NegativeArraySizeException")`**。
- `**arraylength`（0xbe）**：弹出数组引用，压入 `**getArrayLength()`**；`null` 抛 `**NullPointerException**`；非数组类型抛 `**std::runtime_error**`。
- **GC**：`GarbageCollector::markObjectGraph` 对 **非 `isPrimitiveArray()`** 且 `arrayLength > 0` 的对象，会递归标记各槽中的 `Object*`（多维数组、`Object[]` 同理）。

## 内置桩（可扩展）

### java.lang.System

- `registerNatives`、`setIn0` / `setOut0` / `setErr0`：空或占位。
- `nanoTime`、`currentTimeMillis`：`chrono`。
- `arraycopy`：源、目标 **类描述符相同** 时，原始类型数组按元素宽度 `memmove`；**引用 / 多维数组**（`[L...`、`[[` 等）按 `sizeof(Object*)` 逐槽拷贝。越界 / 空指针 / 类型不一致时静默返回（未抛 Java 异常）。
- `identityHashCode`、`mapLibraryName`：基础实现。

### java.lang.Object

- `hashCode`、`getClass`（`java/lang/Class` **镜像对象**：`Object::javaClass` 存入镜像 `data`）、`clone`（暂返回同一引用）、`notify` / `notifyAll` / `wait0`：空或桩。

### java.lang.String

- `intern`：返回 `this`。

### java.lang.Class

- `registerNatives` 及大量 **反射/注解相关 native**：多数返回 **null / 0 / 空操作**；`isInstance` / `isAssignableFrom` / `isInterface` / `isArray` / `isPrimitive` / `getSuperclass` / `getModifiers` / `initClassName` / `getPrimitiveClass` 等按 `JavaClass` 元数据或镜像做近似实现。`**forName0`**：已注册 `(Ljava/lang/String;ZLjava/lang/ClassLoader;Ljava/lang/Class;)Ljava/lang/Class;`，用当前帧的 `ClassLoader` 做 `loadClass`（Java 侧 `ClassLoader` 参数尚未绑定到运行时加载器）；`initialize` 与 **ClassNotFoundException** 仍为占位（失败返回 `null` 而非抛异常）。

### java.lang.Thread

- `registerNatives`、`currentThread`（单例桩对象）、`currentCarrierThread` 等同、`sleepNanos0` / `yield0` 等：空或简单返回值。

### java.lang.ClassLoader

- `registerNatives`：空。

## 相关修正

`Method::isNative()`（及 `isSynchronized` / `isAbstract` 等）原为 `flags && ACC_`* 逻辑与，易误判；已改为 **位与** `(flags & ACC_*) != 0`。