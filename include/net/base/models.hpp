#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace uranus::net {
struct Message {
    Message() : Message(0, "", "") {}

    Message(const std::uint64_t code, std::string_view msg, std::string_view data)
        : code_(code), message_(msg), data_(data) {}

    std::uint64_t code_;
    std::string   message_;
    std::string   data_;
};

// to_json 和 from_json 需要在同一命名空间下
void to_json(nlohmann::json &data, const Message &msg) {
    data["code"]    = msg.code_;
    data["message"] = msg.message_;
    data["data"]    = msg.data_;
}

void from_json(const nlohmann::json &data, Message &msg) {
    data.at("code").get_to(msg.code_);
    data.at("message").get_to(msg.message_);
    data.at("data").get_to(msg.data_);
}
}  // namespace uranus::net
