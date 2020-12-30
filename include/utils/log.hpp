#pragma once

#include <chrono>
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string_view>

namespace uranus::utils
{
class logHelper
{
public:
    static auto instance() -> logHelper &
    {
        static logHelper logger;
        return logger;
    }

    template<typename T>
    void error(const T &msg)
    {
        log->error(msg);
    }

    template<typename... T>
    void error(std::string_view fmt, const T &...args)
    {
        log->error(fmt, args...);
    }

    template<typename T>
    void info(const T &msg)
    {
        log->info(msg);
    }

    template<typename... T>
    void info(std::string_view fmt, const T &...args)
    {
        log->info(fmt, args...);
    }

private:
    logHelper()
    {
        log = spdlog::rotating_logger_mt("room", "logs/rotating.log", 1048576 * 5, 3);
        log->set_level(spdlog::level::info);
        spdlog::flush_every(std::chrono::seconds(5));
    }

    ~logHelper() { log->flush(); }

    std::shared_ptr<spdlog::logger> log;
};
}  // namespace uranus::utils
