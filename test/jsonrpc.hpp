#pragma once

#include "jsonrpc/error.hpp"
#include "jsonrpc/notification.hpp"
#include "jsonrpc/request.hpp"
#include "jsonrpc/response.hpp"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json_fwd.hpp>

SCENARIO("json rpc", "[request]") {
    uranus::jsonrpc::Request request;
    GIVEN("request string") {
        auto str = R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})";

        auto result = request.Parse(str);

        CHECK(result);
        CHECK(request.Method() == "subtract");
        CHECK(request.Version() == "2.0");

        auto id = request.Id();
        if (id.index() == 0) {
            CHECK(std::get<int>(id) == 1);
        } else if (id.index() == 1) {
            CHECK(std::get<std::string>(id) == "1");
        }
    }
}

SCENARIO("json rpc", "[response]") {
    GIVEN("response class") {
        uranus::jsonrpc::Response response;

        auto str = response.String();

        CHECK_FALSE(str.empty());
    }

    GIVEN("response result") {
        uranus::jsonrpc::Response response(1);

        auto result    = nlohmann::json::object();
        result["data"] = "sdfsdf";
        result["list"] = {1, 0, 2};

        response.AddResult(result);

        auto str = response.String();

        CHECK_FALSE(str.empty());

        auto lsp = response.LspString();

        CHECK_FALSE(lsp.empty());
    }

    GIVEN("response error") {
        uranus::jsonrpc::Response response;

        uranus::jsonrpc::Error error(uranus::jsonrpc::ErrorCodes::ParseError, "this is a error");

        response.AddError(error.Json());

        auto str = response.String();

        CHECK_FALSE(str.empty());

        auto lsp = response.LspString();
        CHECK_FALSE(lsp.empty());
    }
}

SCENARIO("json rpc", "[notification]") {
    GIVEN("notification no params") {
        uranus::jsonrpc::Notification notice("notice");

        auto str = notice.String();

        CHECK_FALSE(str.empty());

        auto lsp = notice.LspString();
        CHECK_FALSE(lsp.empty());
    }

    GIVEN("notification params") {
        nlohmann::json params;
        params["list"] = {34, 5, 677};

        uranus::jsonrpc::Notification notice("notice", params);

        auto str = notice.String();

        CHECK_FALSE(str.empty());

        auto lsp = notice.LspString();
        CHECK_FALSE(lsp.empty());
    }
}
