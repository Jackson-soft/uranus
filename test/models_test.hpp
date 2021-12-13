#pragma once

#include "net/base/models.hpp"

#include <doctest/doctest.h>

TEST_CASE("json model parse") {
    uranus::net::Message cm{1, "a message", "a data"};
    auto                 v = boost::json::value_from(cm);
    CHECK_EQ(v.as_object().at("code").as_uint64(), 1);
}

TEST_CASE("json model") {
    boost::json::value vj{{"code", std::uint64_t(1)}, {"message", "2"}, {"data", "3"}};
    auto               myObj = boost::json::value_to<uranus::net::Message>(vj);
    CHECK_EQ(myObj.code_, 1);
}
