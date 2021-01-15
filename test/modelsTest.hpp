#pragma once

#include "doctest.h"
#include "models.hpp"
#include <iostream>

TEST_CASE("client parse")
{
    aquarius::clientMsg cm{"1", "2", "3", "4"};
    auto v = boost::json::value_from(cm);
    std::cout << v.as_object().at("cmd").as_string() << std::endl;

    std::cout << boost::json::serialize(v) << std::endl;

    boost::json::value vj{{"cmd", "1"}, {"roomid", "2"}, {"clientid", "3"}, {"msg", "4"}};
    auto myobj = boost::json::value_to<aquarius::clientMsg>(vj);
    std::cout << myobj.clientid << std::endl;
}