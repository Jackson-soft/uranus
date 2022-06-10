#pragma once

#include "redis/redis.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("redis test") {
    uranus::redis::Redis redis;
    REQUIRE(redis.Dial("127.0.0.1", 6379) == true);
    REQUIRE(redis.Dial() == true);
    REQUIRE(redis.Ping() == 0);
}