#pragma once

#include "doctest/doctest.h"
#include "net/models.hpp"
#include <iostream>

TEST_CASE("client parse")
{
    uranus::net::Return cm{1, "2", "3"};
    auto v = boost::json::value_from(cm);
    std::cout << v.as_object().at("code").as_uint64()  << std::endl;

    std::cout << boost::json::serialize(v) << std::endl;

    boost::json::value vj{{"code", std::uint64_t(1)}, {"message", "2"}, {"data", "3"}};
    auto myobj = boost::json::value_to<uranus::net::Return>(vj);
    std::cout << myobj.code << std::endl;
}