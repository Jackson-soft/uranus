#pragma once

#include <mutex>

namespace Uranus::Database
{
class Pool
{
public:
    static auto instance() -> Pool &
    {
        static Pool instance;
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
}  // namespace Uranus::Database