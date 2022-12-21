#pragma once

#include "boost/asio/ip/tcp.hpp"
#include "redis/options.hpp"
#include "redis/session.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::redis {
class Client {
public:
    Client() : session_(ioContext_) {}

    ~Client() = default;

    auto Dial(std::string_view hostname = "127.0.0.1", const std::uint16_t port = 6379) -> bool {
        if (hostname.empty()) {
            return false;
        }

        return session_.Connect(hostname, port);
    }

    auto Ping() -> bool {
        session_.Write("PING");
        return true;
    }

    auto Set(std::string_view key, std::string_view value) -> bool {
        if (key.empty() || value.empty()) {
            return false;
        }
        return true;
    }

    auto Get(std::string_view key) -> std::optional<std::string> {
        if (key.empty()) {
            return std::nullopt;
        }
        return std::nullopt;
    }

private:
    boost::asio::io_context ioContext_;
    Session                 session_;
    Options                 options_;
};
}  // namespace uranus::redis
