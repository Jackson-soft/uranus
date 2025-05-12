#pragma once

// https://www.jsonrpc.org/specification

#include "version.hpp"

#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::jsonrpc {
class Notification {
public:
    explicit Notification(const std::string_view method) : method_(method) {}

    Notification(const std::string_view method, nlohmann::json params) : method_(method), params_(std::move(params)) {}

    ~Notification() = default;

    auto String() -> std::string {
        nlohmann::json result;

        result["jsonrpc"] = jsonrpc_;

        result["method"] = method_;

        if (!params_.empty()) {
            result["params"] = params_;
        }

        return result.dump(-1, ' ', true, nlohmann::detail::error_handler_t::ignore);
    }

private:
    /**
     * 待触发的 method
     */
    std::string method_;

    std::string jsonrpc_{Version()};

    /**
     * 通知的参数
     */
    nlohmann::json params_{};
};
}  // namespace uranus::jsonrpc
