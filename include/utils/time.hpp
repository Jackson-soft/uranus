#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace uranus::utils {
// 时间操作
inline auto NowTime() -> std::tm {
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return *std::localtime(&now_c);
}

// 获取当前时间 格式： YYYY-MM-DD HH:MM:SS
inline auto CurrentTime() -> std::string {
    auto              now = NowTime();
    std::stringstream ss;
    ss << std::put_time(&now, "%F %T");
    return ss.str();
}

// 获取当前日期 格式：YYYY-MM-DD
inline auto CurrentDay() -> std::string {
    auto              now = NowTime();
    std::stringstream ss;
    ss << std::put_time(&now, "%F");
    return ss.str();
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
