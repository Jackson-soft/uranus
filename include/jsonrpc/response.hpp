#pragma once

/*
When a rpc call is made, the Server MUST reply with a Response, except for in the case of Notifications. The Response is
expressed as a single JSON Object, with the following members:

jsonrpc
A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
result
This member is REQUIRED on success.
This member MUST NOT exist if there was an error invoking the method.
The value of this member is determined by the method invoked on the Server.
error
This member is REQUIRED on error.
This member MUST NOT exist if there was no error triggered during invocation.
The value for this member MUST be an Object as defined in section 5.1.
id
This member is REQUIRED.
It MUST be the same as the value of the id member in the Request Object.
If there was an error in detecting the id in the Request object (e.g. Parse error/Invalid Request), it MUST be Null.
Either the result member or error member MUST be included, but both members MUST NOT be included.
*/

#include "version.hpp"

#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::jsonrpc {
class Response {
public:
    Response() = default;

    explicit Response(std::string_view id) : id_(id.data()) {}

    explicit Response(int id) : id_(id) {}

    ~Response() = default;

    void AddResult(nlohmann::json result) {
        result_ = std::move(result);
    }

    void AddError(nlohmann::json error) {
        error_ = std::move(error);
    }

    auto String() -> std::string {
        nlohmann::json result;

        result["jsonrpc"] = jsonrpc_;

        if (id_.index() == 0) {
            result["id"] = std::get<int>(id_);
        } else if (id_.index() == 1) {
            result["id"] = std::get<int>(id_);
        } else {
            result["id"] = nullptr;
        }

        if (!error_.empty()) {
            result["error"] = error_;
        } else {
            result["result"] = result_;
        }

        return result.dump(-1, ' ', true, nlohmann::detail::error_handler_t::ignore);
    }

    // lsp 字符串
    auto LspString() -> std::string {
        auto message = String();
        auto header  = std::format("Content-Length:{}\r\n\r\n", message.size());

        header.append(message);

        return std::move(header);
    }

private:
    /*
    A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
    */
    std::string jsonrpc_{Version()};

    /*
    This member is REQUIRED.
    It MUST be the same as the value of the id member in the Request Object.
    If there was an error in detecting the id in the Request object (e.g. Parse error/Invalid Request), it MUST be Null.
    */
    std::variant<int, std::string, void *> id_{nullptr};

    /*
    This member is REQUIRED on success.
    This member MUST NOT exist if there was an error invoking the method.
    The value of this member is determined by the method invoked on the Server.
    */
    nlohmann::json result_{};

    /*
    This member is REQUIRED on error.
    This member MUST NOT exist if there was no error triggered during invocation.
    The value for this member MUST be an Object as defined in section 5.1.
    */
    nlohmann::json error_{};
};
}  // namespace uranus::jsonrpc
