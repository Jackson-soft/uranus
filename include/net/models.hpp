#pragma once

#include <boost/json/src.hpp>
#include <cstdint>
#include <string>
#include <string_view>

namespace uranus::net
{
class returned
{
public:
    returned(std::uint32_t c, std::string_view m, std::string_view d): code(c), message(m), data(d) {}

    std::uint32_t code;
    std::string message;
    std::string data;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, returned const &c)
{
    jv = {{"code", c.code}, {"message", c.message}, {"data", c.data}};
}

returned tag_invoke(boost::json::value_to_tag<returned>, boost::json::value const &jv)
{
    auto &jo = jv.as_object();
    return returned(jo.at("code").as_uint64(), jo.at("message").as_string().data(), jo.at("data").as_string().data());
}
}  // namespace uranus::net