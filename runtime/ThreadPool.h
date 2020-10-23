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
#include <functional>
#include <future>
#include <deque>

using namespace std;
namespace Runtime{
    class ThreadPool {
    public:
        void start(unsigned int numberOfThreads) {
            for(unsigned int i{0}; i < numberOfThreads; i++) {
                threads.push_back(std::thread(&ThreadPool::worker, this));
            }
        }

        void worker() {
            while (!done) {
                std::function<void()> work;
                {
                    std::unique_lock<std::mutex> ul(m);
                    cv.wait(ul,[&] { return ( hasWork() || done );});
                    work = pull();
                }

                if(done) break;
                work();
            }
        }

        void add(std::function<void()> work) {
            {
                std::lock_guard<std::mutex> guard(m);
                workQueue.push_front(work);
            }
            cv.notify_one();
        }

        template<class F, class... Args>
        auto submit(F&& task_function, Args&&... args)
        {
            using T = decltype(task_function(args...));

            auto task = std::make_shared<std::packaged_task<T()>> (
                    std::bind(std::forward<F>(task_function), std::forward<Args>(args)...)
            );

            std::future<T> result = task->get_future();
            auto work = [task] () { (*task)(); };

            {
                std::lock_guard<std::mutex> guard(m);
                workQueue.push_front(work);
            }

            cv.notify_one();

            return result;
        }

        bool hasWork() {
            return !workQueue.empty();
        }

        std::function<void()> pull() {
            if(workQueue.empty()) {
                return []{};
            }

            auto work = workQueue.back();
            workQueue.pop_back();
            return work;
        }

        ~ThreadPool() {
            done = true;
            cv.notify_all();
            for(auto& t:threads) {
                t.join();
            }
        }
        virtual void finalize(){
            this->done=true;
        };

        virtual void runPendingWork(){
            while (!done){
                m.lock();
                if(!workQueue.empty()){
                   auto task=std::move( workQueue.front());
                   workQueue.pop_front();
                   m.unlock();
                   task();
                }else{
                    m.unlock();
                    std::this_thread::yield();
                }

            }
        };
        std::vector<std::thread> threads;
        std::deque<std::function<void()>> workQueue;
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
