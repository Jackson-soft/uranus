#pragma once

#include "database/dsn.hpp"
#include <doctest/doctest.h>

#include <iostream>

TEST_CASE("dsn parse")
{
    auto str{"postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full"};
    uranus::database::DSN dsn;

    dsn.parse(str);
    std::cout << dsn.driver() << std::endl;

    CHECK_EQ(dsn.driver(), "postgres");
    CHECK_EQ(dsn.user(), "bob");
    CHECK_EQ(dsn.password(), "secret");
    CHECK_EQ(dsn.host(), "1.2.3.4");
    CHECK_EQ(dsn.port(), std::uint16_t(5432));
    CHECK_EQ(dsn.dbName(), "mydb");
}
