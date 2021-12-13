#pragma once

#include <atomic>
#include <mutex>
#include <queue>

namespace uranus::database {
class Pool
{
public:
    static auto Get() -> Pool & {
        static Pool instance;
        return instance;
    }

    auto Init() -> bool {
        std::call_once(flag_, [this]() {
            this->create();
        });
        return ready_.load();
    }

private:
    void create() {
        ready_.store(false);
    }

    std::mutex       mutex_;
    std::once_flag   flag_;
    std::atomic_bool ready_;
};
}  // namespace uranus::database
