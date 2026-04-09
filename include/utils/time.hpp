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
    const auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

// 获取当前时间戳(毫秒精度)
inline auto UnixMilli() -> std::int64_t {
    const auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
}  // namespace uranus::utils
