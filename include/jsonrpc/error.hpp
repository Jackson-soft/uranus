#pragma once

/*

When a rpc call encounters an error, the Response Object MUST contain the error member with a value that is a Object
with the following members:

code
A Number that indicates the error type that occurred.
This MUST be an integer.
message
A String providing a short description of the error.
The message SHOULD be limited to a concise single sentence.
data
A Primitive or Structured value that contains additional information about the error.
This may be omitted.
The value of this member is defined by the Server (e.g. detailed error information, nested errors etc.).

-32700 ---> parse error. not well formed
-32701 ---> parse error. unsupported encoding
-32702 ---> parse error. invalid character for encoding
-32600 ---> server error. invalid xml-rpc. not conforming to spec.
-32601 ---> server error. requested method not found
-32602 ---> server error. invalid method parameters
-32603 ---> server error. internal xml-rpc error
-32500 ---> application error
-32400 ---> system error
-32300 ---> transport error
*/

#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>

namespace uranus::jsonrpc {
enum class ErrorCodes : int {
    // 由 JSON RPC 定义
    ParseError     = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams  = -32602,
    InternalError  = -32603,
};

class Error {
public:
    Error() = default;

    explicit Error(std::int32_t code) : Error(code, "") {}

    Error(std::int32_t code, std::string_view message) : code_{code}, message_{message} {}

    ~Error() = default;

    auto String() -> std::string {
        return Json().dump(-1, ' ', true, nlohmann::detail::error_handler_t::ignore);
    }

    auto Json() -> nlohmann::json {
        auto result       = nlohmann::json::object();
        result["code"]    = code_;
        result["message"] = message_;
        // result["data"]    = data_;

        return result;
    }

private:
    /*
    A Number that indicates the error type that occurred.
    This MUST be an integer.
    */
    std::int32_t code_{-32000};
    /*
    A String providing a short description of the error.
    The message SHOULD be limited to a concise single sentence.
    */
    std::string message_{};
    /*
    A Primitive or Structured value that contains additional information about the error.
    This may be omitted.
    The value of this member is defined by the Server (e.g. detailed error information, nested errors etc.).
    */
    nlohmann::json data_;
};
}  // namespace uranus::jsonrpc
