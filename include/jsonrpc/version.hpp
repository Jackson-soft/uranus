#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <variant>

namespace uranus::jsonrpc {
// A String specifying the version of the JSON-RPC protocol. MUST be exactly
// "2.0".
inline constexpr std::string_view kVersion = "2.0";

// ID type: int, string, or nullptr_t (null)
using Id = std::variant<int, std::string, std::nullptr_t>;
}  // namespace uranus::jsonrpc