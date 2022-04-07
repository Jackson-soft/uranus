#pragma once

#include <boost/algorithm/string/case_conv.hpp>
#include <cstddef>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

namespace uranus::utils {
class LogHelper {
public:
    static auto Instance() -> LogHelper & {
        static LogHelper logger;
        return logger;
    }

    void Init(std::string_view level   = "info",
              std::size_t      maxSize = static_cast<std::size_t>(1024 * 1024 * 20),
              std::string_view file    = "logs/rotating.log",
              std::size_t      maxFile = 3,
              std::string_view name    = "app") {
        // Create a file rotating logger with 20mb size max and 3 rotated files.
        log_ = spdlog::rotating_logger_mt(name.data(), file.data(), maxSize, maxFile);
        auto        lvl{spdlog::level::info};
        std::string ll{level};
        boost::to_lower(ll);
        if (ll == "info") {
            lvl = spdlog::level::info;
        } else if (ll == "error") {
            lvl = spdlog::level::err;
        } else if (ll == "debug") {
            lvl = spdlog::level::debug;
        }
        log_->set_level(lvl);
        log_->flush_on(spdlog::level::debug);
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
        spdlog::shutdown();
    }

    std::shared_ptr<spdlog::logger> log_;
};
}  // namespace uranus::utils
