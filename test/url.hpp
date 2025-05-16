#pragma once

#include "net/base/uri.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("url test") {
    GIVEN("http url test") {
        const auto      *str{"http://example.org/absolute/URI/with/absolute/path/to/resource.txt"};
        uranus::net::URI url;
        CHECK(url.Parse(str));
    }

    GIVEN("ftp url test") {
        const auto      *str{"ftp://example.org/resource.txt"};
        uranus::net::URI url;
        CHECK(url.Parse(str));
    }
}