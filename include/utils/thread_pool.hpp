#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace uranus::utils {
class ThreadPool {
public:
    ThreadPool() = default;

    // the constructor just launches some amount of workers
    explicit ThreadPool(size_t threads) : stop(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    // add new work item to the pool
    template<class F, class... Args>
    auto Enqueue(F &&f, Args &&...args) -> std::future<typename std::invoke_result<F(Args...)>::type> {
        using ret_type
            = std::future<typename std::invoke_result<F, Args...>::type>;  // typename 此处加不加均可以的，下面同
        std::function<typename std::invoke_result<F, Args...>::type()> func
            = std::bind(std::forward<F>(f),
                        std::forward<Args>(args)...);  // 连接函数和参数定义，特殊函数类型，避免左右值错误

        auto task = std::make_shared<std::packaged_task<typename std::invoke_result<F, Args...>::type()>>(func);

        std::function<void()> warpper_func = [task]() {
            (*task)();
        };
        ret_type res = task->get_future();
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(warpper_func);
        condition.notify_one();
        return res;
    }

    // the destructor joins all threads
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();

        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex              queue_mutex;
    std::condition_variable condition;
    bool                    stop;
};
}  // namespace uranus::utils