#pragma once

#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string_view>

namespace uranus::utils {
class LogHelper {
public:
    static auto Instance() -> LogHelper & {
        static LogHelper logger;
        return logger;
    }

    template<typename T>
    void Error(const T &msg) {
        log_->error(msg);
    }

    template<typename... T>
    void Error(std::string_view fmt, T &&...args) {
        log_->error(fmt, std::forward<T>(args)...);
    }

    template<typename T>
    void Info(const T &msg) {
        log_->info(msg);
    }

    template<typename... T>
    void Info(std::string_view fmt, T &&...args) {
        log_->info(fmt, std::forward<T>(args)...);
    }

private:
    LogHelper() {
        log_ = spdlog::rotating_logger_mt("app", "logs/rotating.log", 1048576 * 5, 3);
        log_->set_level(spdlog::level::info);
        log_->flush_on(spdlog::level::info);
        // log->set_pattern("[%Y-%m-%d %T.%e] [%@] [%!] [%l]: %v");
    }

    ~LogHelper() {
        log_->flush();
        spdlog::shutdown();
    }

    std::shared_ptr<spdlog::logger> log_;
};
}  // namespace uranus::utils
