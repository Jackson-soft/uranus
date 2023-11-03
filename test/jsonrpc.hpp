#pragma once

#include "jsonrpc/request.hpp"
#include "jsonrpc/response.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("json rpc", "[request]") {
    uranus::jsonrpc::Request request;
    GIVEN("request string") {
        auto str = R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})";

        auto result = request.Parse(str);

        CHECK(result);
        CHECK(request.Method() == "subtract");
    }
}