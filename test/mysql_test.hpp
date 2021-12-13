#pragma once

#include "database/mysql.hpp"
#include "database/pool.hpp"

#include <doctest/doctest.h>

TEST_SUITE("mysql test") {
    TEST_CASE("mysql args test") {
        uranus::database::MySQL mysql;
        CHECK_EQ(mysql.Connect("root", "ruisi_112", "ruisi", "127.0.0.1", 3306), true);
        CHECK_EQ(mysql.Ping(), true);
        std::vector<std::map<std::string, std::string>> rs;
        CHECK_EQ(mysql.Query("select * from demo", rs), true);
        CHECK(rs.size() > 0);
        CHECK_EQ(mysql.Execute("update demo set age = 21 where id = 1"), true);
    }

    TEST_CASE("mysql tcp dsn test") {
        uranus::database::MySQL mysql;
        CHECK_EQ(mysql.Connect("root:ruisi_112@tcp(127.0.0.1:3306)/ruisi"), true);
        CHECK_EQ(mysql.Ping(), true);
        std::vector<std::map<std::string, std::string>> rs;
        CHECK_EQ(mysql.Query("select * from demo", rs), true);
        CHECK(rs.size() > 0);
        CHECK_EQ(mysql.Execute("update demo set age = 21 where id = 1"), true);
    }
}