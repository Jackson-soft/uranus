#pragma once

#include <mutex>

namespace uranus::database
{
class pool
{
public:
    static auto instance() -> pool &
    {
        static pool instance;
        return instance;
    }

    void init()
    {
        // std::call_once(flag, create());
    }

private:
    void create() {}

    std::mutex mtx;
    std::once_flag flag;
};
}  // namespace uranus::database