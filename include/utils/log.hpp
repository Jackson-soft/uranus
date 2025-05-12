#pragma once

#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <filesystem>

namespace uranus::utils {
class LogHelper {
public:
    static auto Instance() -> LogHelper & {
        static LogHelper instance;
        return instance;
    }

    void Initialize(const std::filesystem::path& file    = "logs/rotating.log",
                   const std::string_view level   = "info",
                   const std::string_view name    = "app",
                   const std::size_t      maxSize = static_cast<std::size_t>(1024 * 1024 * 20),
                   const std::size_t      maxFile = 3) {
        // Create a file rotating logger with 20mb size max and 3 rotated files.
        log_ = spdlog::rotating_logger_mt(name.data(), file, maxSize, maxFile);
        auto              lvl{spdlog::level::info};
        if (std::string const ll{level}; ll == "info") {
            lvl = spdlog::level::info;
        } else if (ll == "error") {
            lvl = spdlog::level::err;
        } else if (ll == "debug") {
            lvl = spdlog::level::debug;
        }

        log_->set_level(lvl);
        // log_->flush_on(spdlog::level::debug);
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
    LogHelper() = default;

    ~LogHelper() {
        log_->flush();
    }

    std::shared_ptr<spdlog::logger> log_;
};
}  // namespace uranus::utils
