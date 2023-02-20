#pragma once

#include "net/base/models.hpp"
#include "nlohmann/json_fwd.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("json model parse") {
    uranus::net::Message msg{1, "a message", "a data"};

    nlohmann::json data = msg;
    REQUIRE(data["code"] == 1);

    nlohmann::json myData;
    myData["code"] = 2;

    auto myMsg = myData.get<uranus::net::Message>();

    REQUIRE(myMsg.code_ == 2);
}
