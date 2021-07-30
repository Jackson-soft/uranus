#pragma once

#include <mutex>

namespace uranus::database {
class Pool {
public:
    static auto get() -> Pool &
    {
        static Pool instance;
        return instance;
    }

    void init()
    {
        // std::call_once(flag, create());
    }

private:
    void           create() {}

    std::mutex     mutex_;
    std::once_flag flag_;
};
}  // namespace uranus::database
