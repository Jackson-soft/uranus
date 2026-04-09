#pragma once

/*
A rpc call is represented by sending a Request object to a Server. The Request
object has the following members:

jsonrpc
A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
method
A String containing the name of the method to be invoked. Method names that
begin with the word rpc followed by a period character (U+002E or ASCII 46) are
reserved for rpc-internal methods and extensions and MUST NOT be used for
anything else. params A Structured value that holds the parameter values to be
used during the invocation of the method. This member MAY be omitted. id An
identifier established by the Client that MUST contain a String, Number, or NULL
value if included. If it is not included it is assumed to be a notification. The
value SHOULD normally not be Null [1] and Numbers SHOULD NOT contain fractional
parts [2] The Server MUST reply with the same value in the Response object if
included. This member is used to correlate the context between the two objects.

[1] The use of Null as a value for the id member in a Request object is
discouraged, because this specification uses a value of Null for Responses with
an unknown id. Also, because JSON-RPC 1.0 uses an id value of Null for
Notifications this could cause confusion in handling.

[2] Fractional parts may be problematic, since many decimal fractions cannot be
represented exactly as binary fractions.
*/

#include "version.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <variant>

namespace uranus::jsonrpc {
class Request {
public:
    Request()  = default;
    ~Request() = default;

    // 解析 JSON-RPC 请求，返回 true 表示成功; 失败时可通过 ErrorMessage()
    // 获取错误信息
    auto Parse(std::string_view msg) -> bool {
        nlohmann::json data;
        try {
            data = nlohmann::json::parse(msg);
        } catch (const nlohmann::json::parse_error &e) {
            error_ = e.what();
            return false;
        }

        if (!data.is_object()) {
            error_ = "request must be a JSON object";
            return false;
        }

        // jsonrpc 字段必须为 "2.0"
        if (!data.contains("jsonrpc") || !data["jsonrpc"].is_string()
            || data["jsonrpc"].get<std::string>() != kVersion) {
            error_ = R"(missing or invalid "jsonrpc" field, must be "2.0")";
            return false;
        }
        jsonrpc_ = data["jsonrpc"].get<std::string>();

        // method 字段必须存在且为字符串
        if (!data.contains("method") || !data["method"].is_string()) {
            error_ = R"(missing or invalid "method" field)";
            return false;
        }
        method_ = data["method"].get<std::string>();

        // id: 可选，String / Number / Null
        if (data.contains("id")) {
            if (data["id"].is_number_integer()) {
                id_ = data["id"].get<int>();
            } else if (data["id"].is_string()) {
                id_ = data["id"].get<std::string>();
            } else if (data["id"].is_null()) {
                id_ = nullptr;
            } else {
                error_ = R"("id" must be a string, integer, or null)";
                return false;
            }
        }

        // params: 可选，必须是 object 或 array
        if (data.contains("params")) {
            if (data["params"].is_object() || data["params"].is_array()) {
                params_ = data["params"];
            } else {
                error_ = R"("params" must be an object or array)";
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto Params() const -> const nlohmann::json & {
        return params_;
    }

    [[nodiscard]] auto Method() const -> const std::string & {
        return method_;
    }

    [[nodiscard]] auto Version() const -> const std::string & {
        return jsonrpc_;
    }

    [[nodiscard]] auto GetId() const -> const Id & {
        return id_;
    }

    [[nodiscard]] auto IsNotification() const -> bool {
        return std::holds_alternative<std::nullptr_t>(id_) && !method_.empty();
    }

    [[nodiscard]] auto ErrorMessage() const -> const std::string & {
        return error_;
    }

private:
    std::string    jsonrpc_;
    std::string    method_;
    Id             id_{nullptr};
    nlohmann::json params_{};
    std::string    error_;
};
}  // namespace uranus::jsonrpc
