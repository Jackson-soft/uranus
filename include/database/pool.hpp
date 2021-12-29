#pragma once

#include <atomic>
#include <mutex>
#include <queue>
#include "database/database.hpp"
#include <string_view>

namespace uranus::database {
class Pool
{
public:
    static auto Get() -> Pool & {
        static Pool instance;
        return instance;
    }

    auto Init(std::string_view dsn) -> bool {
        std::call_once(flag_, [this]() {
            this->create();
        });
        return ready_.load();
    }

private:
    void create() {
        ready_.store(true);
    }

    std::mutex       mutex_;
    std::once_flag   flag_;
    std::atomic_bool ready_{false};
};
}  // namespace uranus::database
