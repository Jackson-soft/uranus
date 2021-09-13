#pragma once

#include "database/dsn.hpp"

#include <cstdint>
#include <doctest/doctest.h>

TEST_SUITE("dsn test")
{
    TEST_CASE("dsn parse")
    {
        const auto *          str{"postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full"};
        uranus::database::DSN dsn;

        CHECK(dsn.parse(str) == true);

        CHECK_EQ(dsn.driver(), "postgres");

        CHECK_EQ(dsn.user(), "bob");
        CHECK_EQ(dsn.password(), "secret");
        CHECK_EQ(dsn.host(), "1.2.3.4");
        CHECK_EQ(dsn.port(), std::uint16_t(5432));
        CHECK_EQ(dsn.dbName(), "mydb");
    }
}