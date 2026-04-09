#pragma once

/*
When a rpc call encounters an error, the Response Object MUST contain the error
member with a value that is a Object with the following members:

code
A Number that indicates the error type that occurred. This MUST be an integer.
message
A String providing a short description of the error. The message SHOULD be
limited to a concise single sentence. data A Primitive or Structured value that
contains additional information about the error. This may be omitted.

Standard error codes:
-32700           Parse error      - Invalid JSON was received by the server.
-32600           Invalid Request  - The JSON sent is not a valid Request object.
-32601           Method not found - The method does not exist / is not
available. -32602           Invalid params   - Invalid method parameter(s).
-32603           Internal error   - Internal JSON-RPC error.
-32000 to -32099 Server error     - Reserved for implementation-defined
server-errors.
*/

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace uranus::jsonrpc {
enum class ErrorCodes : std::int32_t {
    ParseError     = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams  = -32602,
    InternalError  = -32603,
    // -32000 to -32099: 保留给实现自定义的服务端错误
    ServerError = -32000,
};

// 返回标准错误码的默认描述
inline auto DefaultErrorMessage(ErrorCodes code) -> std::string_view {
    switch (code) {
        case ErrorCodes::ParseError:
            return "Parse error";
        case ErrorCodes::InvalidRequest:
            return "Invalid Request";
        case ErrorCodes::MethodNotFound:
            return "Method not found";
        case ErrorCodes::InvalidParams:
            return "Invalid params";
        case ErrorCodes::InternalError:
            return "Internal error";
        case ErrorCodes::ServerError:
            return "Server error";
        default:
            return "Unknown error";
    }
}

class Error {
public:
    Error() = default;

    // 使用错误码构造，自动填充默认消息
    explicit Error(const ErrorCodes code) : code_(code), message_(DefaultErrorMessage(code)) {}

    Error(const ErrorCodes code, const std::string_view message) : code_(code), message_(message) {}

    ~Error() = default;

    void SetData(nlohmann::json data) {
        data_ = std::move(data);
    }

    void SetMessage(const std::string_view message) {
        message_ = message;
    }

    [[nodiscard]] auto Code() const -> ErrorCodes {
        return code_;
    }

    [[nodiscard]] auto Message() const -> const std::string & {
        return message_;
    }

    [[nodiscard]] auto String() const -> std::string {
        return Json().dump(-1, ' ', true, nlohmann::detail::error_handler_t::ignore);
    }

    [[nodiscard]] auto Json() const -> nlohmann::json {
        auto result       = nlohmann::json::object();
        result["code"]    = static_cast<std::int32_t>(code_);
        result["message"] = message_;

        if (!data_.empty()) {
            result["data"] = data_;
        }

        return result;
    }

private:
    ErrorCodes     code_{ErrorCodes::InternalError};
    std::string    message_;
    nlohmann::json data_{};
};
}  // namespace uranus::jsonrpc
