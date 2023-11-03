#pragma once

#include "utils/url.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("url test") {
    GIVEN("http url test") {
        const auto        *str{"http://example.org/absolute/URI/with/absolute/path/to/resource.txt"};
        uranus::utils::URL url;
        CHECK(url.Parse(str));
    }

    GIVEN("ftp url test") {
        const auto        *str{"ftp://example.org/resource.txt"};
        uranus::utils::URL url;
        CHECK(url.Parse(str));
    }
}