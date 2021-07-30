#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace uranus::utils {
//时间操作
inline auto nowTime() -> std::tm
{
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return *std::localtime(&now_c);
}

// 获取当前时间 格式： YYYY-MM-DD HH:MM:SS
inline auto currentTime() -> std::string
{
    auto              now = nowTime();
    std::stringstream ss;
    ss << std::put_time(&now, "%F %T");
    return ss.str();
}

// 获取当前日期 格式：YYYY-MM-DD
inline auto currentDay() -> std::string
{
    auto              now = nowTime();
    std::stringstream ss;
    ss << std::put_time(&now, "%F");
    return ss.str();
}

// 获取当前时间戳(秒精度)
inline auto unixTime() -> std::int64_t
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp
        = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());

    auto tmp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());

    return tmp.count();
}

// 获取当前时间戳(毫秒精度)
inline auto milUnixTime() -> std::int64_t
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp
        = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());

    return tmp.count();
}
}  // namespace uranus::utils
