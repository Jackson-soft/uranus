#pragma once

#include "redis/redis.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("redis test") {
    uranus::redis::Client client;
    client.Dial();
    client.Ping();
    // client.Set("myKey", "myValue");
    client.Get("myKey");
    client.Get("dff");
}