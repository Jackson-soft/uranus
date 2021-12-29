#pragma once

#include <boost/json/src.hpp>
#include <cstdint>
#include <string>
#include <string_view>

namespace uranus::net {
class Message
{
public:
    Message(const std::uint64_t c, std::string_view m, std::string_view d) : code_(c), message_(m), data_(d) {}

    std::uint64_t code_;
    std::string   message_;
    std::string   data_;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Message const &c) {
    jv = {{"code", c.code_}, {"message", c.message_}, {"data", c.data_}};
}

Message tag_invoke(boost::json::value_to_tag<Message>, boost::json::value const &jv) {
    const auto &jo = jv.as_object();
    return Message(jo.at("code").as_uint64(), jo.at("message").as_string().data(), jo.at("data").as_string().data());
}
}  // namespace uranus::net
