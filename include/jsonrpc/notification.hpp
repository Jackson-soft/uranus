#pragma once

// https://www.jsonrpc.org/specification

#include "version.hpp"

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::jsonrpc {
class Notification {
public:
    explicit Notification(std::string_view method) : method_(method) {}

    Notification(std::string_view method, nlohmann::json params) : method_(method), params_(std::move(params)) {}

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

    // lsp 字符串
    auto LspString() -> std::string {
        auto message = String();
        auto header  = fmt::format("Content-Length:{}\r\n\r\n", message.size());

        header.append(message);

        return std::move(header);
    }

private:
    std::string jsonrpc_{Version()};

    /**
     * 待触发的 method
     */
    std::string method_;

    /**
     * 通知的参数
     */
    nlohmann::json params_{};
};
}  // namespace uranus::jsonrpc
