#pragma once

#include <chrono>
#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string_view>

namespace uranus::utils {
class LogHelper {
public:
    static auto get() -> LogHelper &
    {
        static LogHelper logger;
        return logger;
    }

    template<typename T>
    void error(const T &msg)
    {
        log_->error(msg);
    }

    template<typename... T>
    void error(std::string_view fmt, const T &...args)
    {
        log_->error(fmt, args...);
    }

    template<typename T>
    void info(const T &msg)
    {
        log_->info(msg);
    }

    template<typename... T>
    void info(std::string_view fmt, const T &...args)
    {
        log_->info(fmt, args...);
    }

private:
    LogHelper()
    {
        log_ = spdlog::rotating_logger_mt("app", "logs/rotating.log", 1048576 * 5, 3);
        log_->set_level(spdlog::level::info);
        log_->flush_on(spdlog::level::info);
        // log->set_pattern("[%Y-%m-%d %T.%e] [%@] [%!] [%l]: %v");
    }

    ~LogHelper()
    {
        spdlog::drop_all();
    }

    std::shared_ptr<spdlog::logger> log_;
};
}  // namespace uranus::utils
