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
    explicit ThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mutex_);
                        this->condition_.wait(lock, [this] {
                            return this->stop_ || !this->tasks_.empty();
                        });
                        if (this->stop_ && this->tasks_.empty()) {
                            return;
                        }
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }

                    task();
                }
            });
        }
    }

    // add new work item to the pool
    template<class Function, class... Args, typename ReturnType = std::invoke_result_t<Function &&, Args &&...>>
    requires std::invocable<Function, Args...>
    auto Enqueue(Function &&fun, Args &&...args) -> std::future<ReturnType> {
        std::function<ReturnType> func
            = std::bind(std::forward<Function>(fun),
                        std::forward<Args>(args)...);  // 连接函数和参数定义，特殊函数类型，避免左右值错误

        auto task = std::make_shared<std::packaged_task<ReturnType>>(func);

        std::function<void()> warpper_func = [task]() {
            (*task)();
        };
        ReturnType res = task->get_future();
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.emplace(warpper_func);
        condition_.notify_one();
        return res;
    }

    // the destructor joins all threads
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();

        for (std::thread &worker : workers_) {
            worker.join();
        }
    }

    ThreadPool(const ThreadPool &)                     = delete;
    auto operator=(const ThreadPool &) -> ThreadPool & = delete;

private:
    std::vector<std::thread>          workers_;  // need to keep track of threads so we can join them
    std::queue<std::function<void()>> tasks_;    // the task queue
    std::mutex                        mutex_;
    std::condition_variable           condition_;  // synchronization
    bool                              stop_{false};
};
}  // namespace uranus::utils
