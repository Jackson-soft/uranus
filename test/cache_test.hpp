#pragma once

#include "redis/redis.hpp"

#include <doctest/doctest.h>

TEST_CASE("redis test")
{
    uranus::redis::Redis redis;
    CHECK(redis.Dial("127.0.0.1", 6379) == true);
    CHECK(redis.Dial() == true);
    CHECK(redis.Ping() == 0);
}