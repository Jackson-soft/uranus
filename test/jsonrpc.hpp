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

        auto id = request.GetId();
        if (std::holds_alternative<int>(id)) {
            CHECK(std::get<int>(id) == 1);
        } else if (std::holds_alternative<std::string>(id)) {
            CHECK(std::get<std::string>(id) == "1");
        }
    }

    GIVEN("request with object params") {
        auto str = R"({"jsonrpc": "2.0", "method": "update", "params": {"key": "value"}, "id": "abc"})";
        CHECK(request.Parse(str));
        CHECK(request.Params().is_object());
        CHECK(std::holds_alternative<std::string>(request.GetId()));
    }

    GIVEN("request with array params") {
        auto str = R"({"jsonrpc": "2.0", "method": "update", "params": [1, 2, 3], "id": 1})";
        CHECK(request.Parse(str));
        CHECK(request.Params().is_array());
    }

    GIVEN("invalid json") {
        auto str = R"({not valid json)";
        CHECK_FALSE(request.Parse(str));
        CHECK_FALSE(request.ErrorMessage().empty());
    }

    GIVEN("missing jsonrpc field") {
        auto str = R"({"method": "test", "id": 1})";
        CHECK_FALSE(request.Parse(str));
    }

    GIVEN("missing method field") {
        auto str = R"({"jsonrpc": "2.0", "id": 1})";
        CHECK_FALSE(request.Parse(str));
    }

    GIVEN("invalid params type") {
        auto str = R"({"jsonrpc": "2.0", "method": "test", "params": "bad", "id": 1})";
        CHECK_FALSE(request.Parse(str));
    }

    GIVEN("notification (no id)") {
        auto str = R"({"jsonrpc": "2.0", "method": "notify"})";
        CHECK(request.Parse(str));
        CHECK(request.IsNotification());
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

        response.SetResult(result);

        auto str = response.String();

        CHECK_FALSE(str.empty());
    }

    GIVEN("response with id=0") {
        uranus::jsonrpc::Response response(0);
        response.SetResult(nlohmann::json("ok"));

        auto str    = response.String();
        auto parsed = nlohmann::json::parse(str);
        CHECK(parsed["id"] == 0);
    }

    GIVEN("response error") {
        uranus::jsonrpc::Response response;

        uranus::jsonrpc::Error error(uranus::jsonrpc::ErrorCodes::ParseError, "this is a error");

        response.SetError(error.Json());

        auto str = response.String();

        CHECK_FALSE(str.empty());
    }
}

SCENARIO("json rpc", "[error]") {
    GIVEN("error with default message") {
        uranus::jsonrpc::Error error(uranus::jsonrpc::ErrorCodes::MethodNotFound);
        CHECK(error.Message() == "Method not found");
        CHECK(error.Code() == uranus::jsonrpc::ErrorCodes::MethodNotFound);
    }

    GIVEN("error with data") {
        uranus::jsonrpc::Error error(uranus::jsonrpc::ErrorCodes::InternalError, "something broke");
        error.SetData(nlohmann::json{{"detail", "stack trace"}});

        auto j = error.Json();
        CHECK(j.contains("data"));
        CHECK(j["data"]["detail"] == "stack trace");
    }
}

SCENARIO("json rpc", "[notification]") {
    GIVEN("notification no params") {
        uranus::jsonrpc::Notification notice("notice");

        auto str = notice.String();

        CHECK_FALSE(str.empty());
    }

    GIVEN("notification params") {
        nlohmann::json params;
        params["list"] = {34, 5, 677};

        uranus::jsonrpc::Notification notice("notice", params);

        auto str = notice.String();

        CHECK_FALSE(str.empty());
    }

    GIVEN("parse notification") {
        auto str    = R"({"jsonrpc": "2.0", "method": "update", "params": [1, 2, 3]})";
        auto notice = uranus::jsonrpc::Notification::Parse(str);
        CHECK(notice.has_value());
        CHECK(notice->Method() == "update");
        CHECK(notice->Params().is_array());
    }

    GIVEN("parse notification with id should fail") {
        auto str    = R"({"jsonrpc": "2.0", "method": "update", "id": 1})";
        auto notice = uranus::jsonrpc::Notification::Parse(str);
        CHECK_FALSE(notice.has_value());
    }

    GIVEN("parse invalid notification") {
        auto str    = R"({bad json)";
        auto notice = uranus::jsonrpc::Notification::Parse(str);
        CHECK_FALSE(notice.has_value());
    }
}
