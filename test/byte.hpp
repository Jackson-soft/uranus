#pragma once

#include "utils/byte_io.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdio>
#include <fmt/core.h>
#include <vector>

SCENARIO("byte read and write", "byte") {
    GIVEN("socket byte test") {
        WHEN("test byte and char") {
            std::byte myByte{51};
            fmt::print(stdout, "myByte is {}, int is {}\n", myByte, std::to_integer<int>(myByte));

            char myChar{51};
            fmt::print(stdout, "myChar is {}\n", myChar);
            REQUIRE(1 == 1);
        }
    }

    GIVEN("vector test") {
        std::vector<char> data{1, 23, 4, 6, 9};

        REQUIRE(data.size() == 5);
        REQUIRE(data.capacity() >= 5);

        WHEN("erase a i") {
            std::erase(data, 1);

            REQUIRE(data.size() == 4);

            std::erase_if(data, [](char x) -> bool {
                return x == 6;
            });

            REQUIRE(data.size() == 3);
        }
    }
}