//
// Created by zhukovasky on 2020/8/26.
//

#include "Interpret.h"
#include "../runtime/JavaThread.h"
#include "../instructions/base/ByteCodeReader.h"
#include "../instructions/InstructionFactory.h"
#include "../utils/PlatformCompat.h"
#include <memory>

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
            while (true) {
                Runtime::JavaFrame* currentJavaFrame = javaThread->currentFrame();
                if (!currentJavaFrame) {
                    std::cerr << "Warning: No current frame available" << std::endl;
                    break;
                }

                // 获取下一条指令地址
                long nextPc = currentJavaFrame->getNextPc();
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
        } catch (const std::exception& e) {
            std::cerr << "Error in instruction loop: " << e.what() << std::endl;
            throw;
        }
    }

    void Interpret::execByteCode(Runtime::Heap::Method *method, Runtime::JavaHeap *javaHeap) {
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
            loopJavaFrameWithHeap(javaThread.get(), javaHeap);
        } catch (const std::exception& e) {
            std::cerr << "Error executing bytecode with heap: " << e.what() << std::endl;
            throw;
        }
    }

    void Interpret::loopJavaFrameWithHeap(Runtime::JavaThread *javaThread, Runtime::JavaHeap *javaHeap) {
        if (!javaThread || !javaHeap) {
            throw std::runtime_error("JavaThread and JavaHeap cannot be null");
        }

        std::unique_ptr<Instruction::ByteCodeReader> byteCodeReader(
            new Instruction::ByteCodeReader()
        );

        try {
            while (true) {
                Runtime::JavaFrame* currentJavaFrame = javaThread->currentFrame();
                if (!currentJavaFrame) {
                    break;
                }

                long nextPc = currentJavaFrame->getNextPc();
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

                // 可以在这里添加堆管理相关的逻辑
                // 比如垃圾回收检查等
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in instruction loop with heap: " << e.what() << std::endl;
            throw;
        }
    }
}