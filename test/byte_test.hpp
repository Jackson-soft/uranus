#pragma once

#include "utils/byte_io.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdio>
#include <fmt/core.h>
#include <vector>

SCENARIO("byte read and write", "byte") {
    GIVEN("socket byte test") {
        std::vector<char> data{51};
        auto              res = uranus::utils::ReadByte<int>(data);
        fmt::print(stdout, "data is {}\n", res);

        std::byte myByte{51};
        fmt::print(stdout, "myByte is {}, int is {}\n", myByte, std::to_integer<int>(myByte));

        auto bbc = std::byte('a');
        fmt::print(stdout, "bbc is {}, int is {}\n", bbc, std::to_integer<int>(bbc));

        auto bba = std::byte('51');
        fmt::print(stdout, "bba is {}, int is {}\n", bba, std::to_integer<int>(bba));

        char myChar{51};
        fmt::print(stdout, "myChar is {}\n", myChar);
        REQUIRE(1 == 1);
    }
}