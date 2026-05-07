//
// Created by zhukovasky on 2020/8/26.
//

#include "Interpret.h"
#include "../runtime/ExceptionDispatch.h"
#include "../runtime/JvmThrownObject.h"
#include "../runtime/threading/SafepointCoordinator.h"
#include "../runtime/gc/GarbageCollector.h"
#include "../runtime/JavaThread.h"
#include "../runtime/heap/JavaClass.h"
#include "../runtime/native/NativeMethodRegistry.h"
#include "../instructions/base/ByteCodeReader.h"
#include "../instructions/InstructionFactory.h"
#include "../utils/PlatformCompat.h"
#include <memory>

namespace {

/** 带堆执行路径下把 JavaHeap 绑定到当前 JavaThread，退出时清零。 */
struct ActiveJavaHeapGuard {
    Runtime::JavaThread* thread;
    explicit ActiveJavaHeapGuard(Runtime::JavaThread* t, Runtime::JavaHeap* h)
        : thread(t) {
        if (thread) {
            thread->setJavaHeap(h);
        }
    }
    ~ActiveJavaHeapGuard() {
        if (thread) {
            thread->setJavaHeap(nullptr);
        }
    }
};

} // namespace

namespace Interpret{
    Interpret::Interpret() {}

    Interpret::~Interpret() {

    }

    void Interpret::execByteCode(Runtime::Heap::Method *method) {
        if (!method) {
            throw std::runtime_error("Method cannot be null");
        }

        // 使用智能指针管理资源
        std::unique_ptr<Runtime::JavaThread> javaThread(Runtime::JavaThread::newThread(1024));
        if (!javaThread) {
            throw std::runtime_error("Failed to create Java thread");
        }

        try {
            std::unique_ptr<Runtime::JavaFrame> mainJavaFrame(
                new Runtime::JavaFrame(javaThread.get(), method)
            );
            
            javaThread->pushJavaFrame(mainJavaFrame.release()); // 转移所有权给thread
            if (method->getJavaClass() && method->getJavaClass()->getClassLoader()) {
                Runtime::Native::ensureParkTlsBoundForLoader(method->getJavaClass()->getClassLoader());
            }
            loopJavaFrame(javaThread.get());
        } catch (const std::exception& e) {
            std::cerr << "Error executing bytecode: " << e.what() << std::endl;
            throw; // 重新抛出异常而不是直接退出
        }
    }

    void Interpret::loopJavaFrame(Runtime::JavaThread *javaThread) {
        if (!javaThread) {
            throw std::runtime_error("JavaThread cannot be null");
        }

        // 使用智能指针管理ByteCodeReader
        std::unique_ptr<Instruction::ByteCodeReader> byteCodeReader(
            new Instruction::ByteCodeReader()
        );

        try {
            Runtime::Threading::MutatorRegistration mutatorRegistration;
            while (true) {
                Runtime::Threading::SafepointCoordinator::instance().pollSafepoint();
                Runtime::JavaFrame* currentJavaFrame = javaThread->currentFrame();
                if (!currentJavaFrame) {
                    std::cerr << "Warning: No current frame available" << std::endl;
                    break;
                }

                // 获取下一条指令地址
                long nextPc = currentJavaFrame->getNextPc();
                currentJavaFrame->setCurrentInsnBegin(nextPc);
                javaThread->setPcAddress(nextPc);

                // 重置字节码读取器
                Runtime::Heap::Method* method = currentJavaFrame->getMethod();
                if (!method || !method->getCode()) {
                    std::cerr << "Error: Invalid method or bytecode" << std::endl;
                    break;
                }

                byteCodeReader->reset(method->getCode(), nextPc);
                
                // 读取操作码
                u1 opCode = byteCodeReader->readU1();
                
                // 创建指令
                std::unique_ptr<Instruction::Instructions> instructions(
                    Instruction::InstructionFactory::newInstruction(opCode)
                );
                
                if (!instructions) {
                    throw std::runtime_error("Unsupported instruction opcode: " + std::to_string(opCode));
                }

                // 获取操作数
                instructions->fetchOperands(byteCodeReader.get());
                
                // 更新程序计数器
                currentJavaFrame->setNextPc(byteCodeReader->getPc());
                
                // 执行指令
                instructions->execute(currentJavaFrame);

                // 检查是否需要退出
                if (javaThread->getJavaStack()->isEmpty()) {
                    break;
                }
            }
        } catch (const Runtime::JvmThrownObject &jto) {
            if (!Runtime::tryDispatchException(javaThread, jto.thrownObject())) {
                std::cerr << "Uncaught Java exception (no handler)" << std::endl;
                throw;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in instruction loop: " << e.what() << std::endl;
            throw;
        }
    }
                               
    void Interpret::execByteCode(Runtime::Heap::Method *method, Runtime::JavaHeap *javaHeap,
                                 Runtime::ClassLoader *classLoader, Runtime::MethodArea *methodArea) {
        if (!method) {
            throw std::runtime_error("Method cannot be null");
        }
        
        if (!javaHeap) {
            throw std::runtime_error("JavaHeap cannot be null");
        }

        // 使用堆管理的版本
        try {
            std::unique_ptr<Runtime::JavaThread> javaThread(Runtime::JavaThread::newThread(1024));
            if (!javaThread) {
                throw std::runtime_error("Failed to create Java thread");
            }

            std::unique_ptr<Runtime::JavaFrame> mainJavaFrame(
                new Runtime::JavaFrame(javaThread.get(), method)
            );
            
            javaThread->pushJavaFrame(mainJavaFrame.release());
            if (method->getJavaClass() && method->getJavaClass()->getClassLoader()) {
                Runtime::Native::ensureParkTlsBoundForLoader(method->getJavaClass()->getClassLoader());
            }
            ActiveJavaHeapGuard heapBinding(javaThread.get(), javaHeap);
            loopJavaFrameWithHeap(javaThread.get(), javaHeap, classLoader, methodArea);
        } catch (const std::exception& e) {
            std::cerr << "Error executing bytecode with heap: " << e.what() << std::endl;
            throw;
        }
    }

    void Interpret::loopJavaFrameWithHeap(Runtime::JavaThread *javaThread, Runtime::JavaHeap *javaHeap,
                                          Runtime::ClassLoader *classLoader, Runtime::MethodArea *methodArea) {
        if (!javaThread || !javaHeap) {
            throw std::runtime_error("JavaThread and JavaHeap cannot be null");
        }

        std::unique_ptr<Instruction::ByteCodeReader> byteCodeReader(
            new Instruction::ByteCodeReader()
        );

        try {
            Runtime::Threading::MutatorRegistration mutatorRegistration;
            while (true) {
                Runtime::Threading::SafepointCoordinator::instance().pollSafepoint();
                Runtime::JavaFrame* currentJavaFrame = javaThread->currentFrame();
                if (!currentJavaFrame) {
                    break;
                }

                long nextPc = currentJavaFrame->getNextPc();
                currentJavaFrame->setCurrentInsnBegin(nextPc);
                javaThread->setPcAddress(nextPc);

                Runtime::Heap::Method* method = currentJavaFrame->getMethod();
                if (!method || !method->getCode()) {
                    break;
                }

                byteCodeReader->reset(method->getCode(), nextPc);
                u1 opCode = byteCodeReader->readU1();
                
                std::unique_ptr<Instruction::Instructions> instructions(
                    Instruction::InstructionFactory::newInstruction(opCode)
                );
                
                if (!instructions) {
                    throw std::runtime_error("Unsupported instruction opcode: " + std::to_string(opCode));
                }

                instructions->fetchOperands(byteCodeReader.get());
                currentJavaFrame->setNextPc(byteCodeReader->getPc());
                instructions->execute(currentJavaFrame);

                if (javaThread->getJavaStack()->isEmpty()) {
                    break;
                }

                Runtime::GarbageCollector& gc = Runtime::GarbageCollector::instance();
                if (javaHeap->shouldCollect() || gc.shouldGC()) {
                    gc.GC(currentJavaFrame, javaHeap, classLoader, methodArea);
                }
            }
        } catch (const Runtime::JvmThrownObject &jto) {
            if (!Runtime::tryDispatchException(javaThread, jto.thrownObject())) {
                std::cerr << "Uncaught Java exception (no handler)" << std::endl;
                throw;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in instruction loop with heap: " << e.what() << std::endl;
            throw;
        }
    }
}