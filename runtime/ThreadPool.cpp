//
// Created by zhukovasky on 2020/8/12.
//

#include "ThreadPool.h"

namespace Runtime{

    void ThreadPool::start(unsigned int numberOfThreads) {
        for (unsigned int i = 0; i < numberOfThreads; i++) {
            threads.push_back(std::thread(&ThreadPool::worker, this));
        }
    }

    void ThreadPool::worker() {
        while (!done) {
            std::unique_ptr<Runnable> work;
            {
                std::unique_lock<std::mutex> ul(m);
                while (!hasWork() && !done) {
                    cv.wait(ul);
                }
                work = pull();
            }

            if (done) {
                break;
            }
            if (work) {
                work->run();
            }
        }
    }

    bool ThreadPool::hasWork() {
        return !workQueue.empty();
    }

    std::unique_ptr<Runnable> ThreadPool::pull() {
        if (workQueue.empty()) {
            return nullptr;
        }

        auto work = std::move(workQueue.back());
        workQueue.pop_back();
        return work;
    }

    ThreadPool::~ThreadPool() {
        done = true;
        cv.notify_all();
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void ThreadPool::finalize() {
        this->done = true;
    }

    void ThreadPool::runPendingWork() {
        while (!done) {
            m.lock();
            if (!workQueue.empty()) {
                auto task = std::move(workQueue.front());
                workQueue.pop_front();
                m.unlock();
                if (task) {
                    task->run();
                }
            } else {
                m.unlock();
                std::this_thread::yield();
            }
        }
    }

}
