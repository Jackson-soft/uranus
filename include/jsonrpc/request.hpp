#pragma once

/*
A rpc call is represented by sending a Request object to a Server. The Request object has the following members:

jsonrpc
A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
method
A String containing the name of the method to be invoked. Method names that begin with the word rpc followed by a period
character (U+002E or ASCII 46) are reserved for rpc-internal methods and extensions and MUST NOT be used for anything
else. params A Structured value that holds the parameter values to be used during the invocation of the method. This
member MAY be omitted. id An identifier established by the Client that MUST contain a String, Number, or NULL value if
included. If it is not included it is assumed to be a notification. The value SHOULD normally not be Null [1] and
Numbers SHOULD NOT contain fractional parts [2] The Server MUST reply with the same value in the Response object if
included. This member is used to correlate the context between the two objects.

[1] The use of Null as a value for the id member in a Request object is discouraged, because this specification uses a
value of Null for Responses with an unknown id. Also, because JSON-RPC 1.0 uses an id value of Null for Notifications
this could cause confusion in handling.

[2] Fractional parts may be problematic, since many decimal fractions cannot be represented exactly as binary fractions.
*/

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <variant>

namespace uranus::jsonrpc {
class Request {
public:
    Request()  = default;
    ~Request() = default;

    auto Parse(std::string_view msg) -> bool {
        auto data = nlohmann::json::parse(msg);
        if (data["jsonrpc"].is_string()) {
            jsonrpc_ = data["jsonrpc"].get<std::string>();
        }

        if (data["id"].is_number()) {
            id_ = data["id"].get<int>();
        } else if (data["id"].is_string()) {
            id_ = data["id"].get<std::string>();
        }

        if (data["method"].is_string()) {
            method_ = data["method"].get<std::string>();
        }

        if (data["params"].is_object()) {
            params_ = data["params"];
        }
        return true;
    }

    auto Params() -> nlohmann::json {
        return params_;
    }

    auto Method() -> const std::string & {
        return method_;
    }

    auto Version() -> const std::string & {
        return jsonrpc_;
    }

    auto Id() -> std::variant<int, std::string, void *> {
        return id_;
    }

private:
    // A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
    std::string jsonrpc_{Version()};

    /*
    A String containing the name of the method to be invoked. Method names that begin with the word rpc followed by a
    period character (U+002E or ASCII 46) are reserved for rpc-internal methods and extensions and MUST NOT be used for
    anything else.
    */
    std::string method_{};

    /*
    An identifier established by the Client that MUST contain a String, Number, or NULL value if included. If it is not
    included it is assumed to be a notification. The value SHOULD normally not be Null [1] and Numbers SHOULD NOT
    contain fractional parts
    */
    std::variant<int, std::string, void *> id_{nullptr};

    /*
    A Structured value that holds the parameter values to be used during the invocation of the method. This member MAY
    be omitted.

    params must be objects or arrays
    */
    nlohmann::json params_{};
};
}  // namespace uranus::jsonrpc
