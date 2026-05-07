//
// 当 classpath 中缺少 NPE 等类时，用内嵌 .class 字节安装最小 java.lang 异常链（依赖已能加载的 java/lang/Object）。
//

#ifndef XJVM_SYNTHETIC_EXCEPTION_STUBS_H
#define XJVM_SYNTHETIC_EXCEPTION_STUBS_H

namespace Runtime {

class ClassLoader;

/** 保证 java/lang 下 NPE / InstantiationException 等桩链可用：优先正常 loadClass；失败则按需 define 内嵌桩类并 link。 */
bool ensureStubJavaLangExceptionChain(ClassLoader *classLoader);

} // namespace Runtime

#endif
