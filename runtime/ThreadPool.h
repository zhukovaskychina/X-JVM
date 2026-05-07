//
// Created by zhukovasky on 2020/8/12.
//

#ifndef JAVAP_THREADPOOL_H
#define JAVAP_THREADPOOL_H

#include "iostream"
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <deque>
#include <type_traits>
#include <utility>

using namespace std;
namespace Runtime{

    /** 可执行工作单元，避免 std::function 在部分链接场景下的 libc++ 符号问题 */
    class Runnable {
    public:
        virtual ~Runnable() = default;
        virtual void run() = 0;
    };

    template <typename F>
    class RunnableLambda final : public Runnable {
    public:
        explicit RunnableLambda(F fn) : fn_(std::move(fn)) {}

        void run() override { fn_(); }

    private:
        F fn_;
    };

    class ThreadPool {
    public:
        void start(unsigned int numberOfThreads);

        void worker();

        template <typename F>
        void add(F&& f) {
            using D = typename std::decay<F>::type;
            {
                std::lock_guard<std::mutex> guard(m);
                workQueue.push_front(
                    std::unique_ptr<Runnable>(new RunnableLambda<D>(std::forward<F>(f))));
            }
            cv.notify_one();
        }

        bool hasWork();

        std::unique_ptr<Runnable> pull();

        virtual ~ThreadPool();

        virtual void finalize();

        virtual void runPendingWork();

        std::vector<std::thread> threads;
        std::deque<std::unique_ptr<Runnable>> workQueue;
        std::atomic<bool> done {false};
        std::mutex m;
        std::condition_variable cv;

    };


    class ExecutorThreadPool:public ThreadPool{
    public:
        ExecutorThreadPool():ThreadPool(){

        }

        void createThread(){
            this->threads.emplace_back(&ThreadPool::runPendingWork,this);
        }

        size_t getThreadNum(){
            return threads.size();
        }

        void storeTaskFuture(shared_future<void> taskFuture){
            this->taskFutures.push_back(taskFuture);
        }
        vector<shared_future<void>> getTaskFutures() {
            return this->taskFutures;
        }
    private:
        vector<shared_future<void>> taskFutures;
    };

}

#endif //JAVAP_THREADPOOL_H
