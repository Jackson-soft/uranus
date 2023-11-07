#pragma once

#include "net/base/models.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("json model parse") {
    WHEN("message to json") {
        uranus::net::Message msg{1, "a message", "a data"};

        nlohmann::json data = msg;

        CHECK(data["code"] == 1);
    }

    WHEN("json to message") {
        nlohmann::json myData;
        myData["code"]    = 2;
        myData["message"] = "mmm";
        myData["data"]    = "ddd";

        auto myMsg = myData.get<uranus::net::Message>();

        CHECK(myMsg.code_ == 2);
    }
}
