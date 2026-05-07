#include "Monitor.h"
#include "../../runtime/threading/ObjectMonitor.h"

#include <stdexcept>

namespace Instruction {

void MonitorEnter::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* obj = javaFrame->getOperandStack()->popObject();
    if (!obj) {
        throw std::runtime_error("MonitorEnter: NullPointerException");
    }
    Runtime::Threading::ObjectMonitorTable::instance().monitorEnter(
        obj, javaFrame->getJavaThread());
}

void MonitorExit::execute(Runtime::JavaFrame* javaFrame) {
    Runtime::Object* obj = javaFrame->getOperandStack()->popObject();
    if (!obj) {
        throw std::runtime_error("MonitorExit: NullPointerException");
    }
    Runtime::Threading::ObjectMonitorTable::instance().monitorExit(
        obj, javaFrame->getJavaThread());
}

} // namespace Instruction
