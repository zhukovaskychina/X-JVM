//
// 按 Code.exception_table 分派异常并展开栈帧。
//

#ifndef XJVM_EXCEPTION_DISPATCH_H
#define XJVM_EXCEPTION_DISPATCH_H

namespace Runtime {

class JavaThread;
class Object;

/** 成功则已清栈顶、压入异常并设置 nextPc；失败表示未捕获（线程已无帧或顶层也未处理）。 */
bool tryDispatchException(JavaThread *thread, Object *exceptionObject);

} // namespace Runtime

#endif
