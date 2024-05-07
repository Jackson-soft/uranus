#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>

namespace uranus::redis {

// set 的扩展参数
struct SetExtra {
    std::uint64_t EX_{0};  // Set the specified expire time, in seconds.
    std::uint64_t PX_{0};  // Set the specified expire time, in milliseconds.
    std::uint64_t EXAT_{
        0};  // timestamp-seconds -- Set the specified Unix time at which the key will expire, in seconds.
    std::uint64_t PXAT_{
        0};  // Stimestamp-milliseconds -- Set the specified Unix time at which the key will expire, in milliseconds.
    bool NX_{false};       // Only set the key if it does not already exist.
    bool XX_{false};       // Only set the key if it already exist.
    bool KeepTTL_{false};  // Retain the time to live associated with the key.
};

// redis客户端基类
class CmdAble {
public:
    CmdAble()          = default;
    virtual ~CmdAble() = default;

    virtual auto Ping() -> bool = 0;
    virtual auto Hello(std::string_view username = "", std::string_view password = "", std::string_view clientName = "")
        -> bool
        = 0;
    virtual auto Set(std::string_view key, const std::any &value, std::unique_ptr<SetExtra> args) -> bool = 0;
    virtual auto Get(std::string_view key) -> std::tuple<std::error_code, std::string>                    = 0;
    virtual auto Del(std::string_view key) -> std::int64_t                                                = 0;
};
}  // namespace uranus::redis
