#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::net {
struct Message {
    Message() : Message(0, "") {}

    Message(const std::uint64_t code, std::string_view msg, nlohmann::json data = nlohmann::json::object())
        : code_(code), message_(msg), data_(std::move(data)) {}

    std::uint64_t  code_;
    std::string    message_;
    nlohmann::json data_;
};

// to_json 和 from_json 需要在同一命名空间下
inline void to_json(nlohmann::json &data, const Message &msg) {
    data = nlohmann::json{{"code", msg.code_}, {"message", msg.message_}, {"data", msg.data_}};
}

inline void from_json(const nlohmann::json &data, Message &msg) {
    data.at("code").get_to(msg.code_);
    data.at("message").get_to(msg.message_);
    data.at("data").get_to(msg.data_);
}
}  // namespace uranus::net
