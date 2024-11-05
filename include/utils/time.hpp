#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <string>

namespace uranus::utils {
// 获取当前时间 格式： YYYY-MM-DD HH:MM:SS
inline auto CurrentTime() -> std::string {
    auto now = std::chrono::system_clock::now();
    return std::format("{0:%Y-%m-%d %H:%M:%S}", now);
}

// 获取当前日期 格式：YYYY-MM-DD
inline auto CurrentDay() -> std::string {
    auto now = std::chrono::system_clock::now();
    return std::format("{0:%F}", now);
}

// 获取当前时间戳(秒精度)
inline auto UnixTime() -> std::int64_t {
    auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());

    return now.time_since_epoch().count();
}

// 获取当前时间戳(毫秒精度)
inline auto UnixMilli() -> std::int64_t {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

    return now.time_since_epoch().count();
}
}  // namespace uranus::utils
