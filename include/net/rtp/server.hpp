#pragma once

#include "net/base/io_pool.hpp"

#include <cstdint>
#include <thread>

namespace uranus::rtp {
class Server
{
public:
    explicit Server(std::uint32_t size = std::thread::hardware_concurrency()) : iocPool_(size) {}

    ~Server() = default;

    void run() {
        iocPool_.run();
    }

private:
    uranus::net::IoPool iocPool_;
};
}  // namespace uranus::rtp
