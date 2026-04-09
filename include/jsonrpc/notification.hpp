#pragma once

// https://www.jsonrpc.org/specification

#include "version.hpp"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::jsonrpc {
class Notification {
public:
    explicit Notification(const std::string_view method) : method_(method) {}

    Notification(const std::string_view method, nlohmann::json params) : method_(method), params_(std::move(params)) {}

    ~Notification() = default;

    // 从 JSON 字符串解析 notification（无 id 的请求）
    static auto Parse(std::string_view msg) -> std::optional<Notification> {
        nlohmann::json data;
        try {
            data = nlohmann::json::parse(msg);
        } catch (const nlohmann::json::parse_error &) {
            return std::nullopt;
        }

        if (!data.is_object())
            return std::nullopt;

        if (!data.contains("jsonrpc") || data["jsonrpc"] != kVersion)
            return std::nullopt;

        if (!data.contains("method") || !data["method"].is_string())
            return std::nullopt;

        // notification 不应包含 id
        if (data.contains("id"))
            return std::nullopt;

        auto method = data["method"].get<std::string>();

        if (data.contains("params") && (data["params"].is_object() || data["params"].is_array())) {
            return Notification(method, data["params"]);
        }

        return Notification(method);
    }

    [[nodiscard]] auto Method() const -> const std::string & {
        return method_;
    }

    [[nodiscard]] auto Params() const -> const nlohmann::json & {
        return params_;
    }

    [[nodiscard]] auto String() const -> std::string {
        nlohmann::json result;

        result["jsonrpc"] = kVersion;
        result["method"]  = method_;

        if (!params_.empty()) {
            result["params"] = params_;
        }

        return result.dump(-1, ' ', true, nlohmann::detail::error_handler_t::ignore);
    }

private:
    std::string    method_;
    nlohmann::json params_{};
};
}  // namespace uranus::jsonrpc
