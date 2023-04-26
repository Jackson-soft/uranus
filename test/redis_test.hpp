#pragma once

#include "redis/command.hpp"
#include "redis/redis.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string>

SCENARIO("redis client") {
    GIVEN("redis command") {
        WHEN("string cmd") {
            auto cmder = uranus::redis::Cmder<std::string>();
        }
        WHEN("int cmd") {
            auto cmder = uranus::redis::Cmder<std::string>();
        }
    }
    GIVEN("a local domain") {
        uranus::redis::Client client;
        THEN("dial") {
            REQUIRE(client.Dial() == true);
            REQUIRE(client.Ping() == true);
        }

        const auto *key   = "myKey";
        const auto *value = "myValue";
        const auto *noKey = "noMyKey";

        WHEN("set a key") {
            client.Set("myKey", "myValue");

            THEN("get key") {
                client.Get(key);
            }
        }

        WHEN("get a no exist key") {
            client.Get(noKey);
        }

        WHEN("delete a key") {}
    }
}
