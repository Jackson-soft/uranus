#pragma once

#include <boost/json/src.hpp>
#include <cstdint>
#include <string>
#include <string_view>

namespace uranus::net
{
class body
{
public:
    body(std::uint32_t c, std::string_view m, std::string_view d): code(c), message(m), data(d) {}

    std::uint32_t code;
    std::string message;
    std::string data;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, body const &c)
{
    jv = {{"code", c.code}, {"message", c.message}, {"data", c.data}};
}

body tag_invoke(boost::json::value_to_tag<body>, boost::json::value const &jv)
{
    auto &jo = jv.as_object();
    return body(jo.at("code").as_uint64(), jo.at("message").as_string().data(), jo.at("data").as_string().data());
}
}  // namespace uranus::net