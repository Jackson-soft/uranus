#pragma once

#include "doctest/doctest.h"
#include "net/base/models.hpp"

#include <iostream>

TEST_CASE("client parse")
{
    uranus::net::Message cm{1, "2", "3"};
    auto                 v = boost::json::value_from(cm);
    std::cout << v.as_object().at("code").as_uint64() << std::endl;

    std::cout << boost::json::serialize(v) << std::endl;

    boost::json::value vj{{"code", std::uint64_t(1)}, {"message", "2"}, {"data", "3"}};
    auto               myobj = boost::json::value_to<uranus::net::Message>(vj);
    std::cout << myobj.code_ << std::endl;
}
