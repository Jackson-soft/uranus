#pragma once

#include <string>

namespace uranus::jsonrpc {
// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
constexpr auto Version() -> std::string {
    return "2.0";
}
}  // namespace uranus::jsonrpc