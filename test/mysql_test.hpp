#pragma once

#include "database/mysql.hpp"
#include "database/pool.hpp"

#include <catch2/catch.hpp>

SCENARIO("mysql test") {
    GIVEN("mysql args test") {
        uranus::database::MySQL mysql;
        REQUIRE(mysql.Connect("root", "ruisi_112", "ruisi", "127.0.0.1", 3306) == true);
        REQUIRE(mysql.Ping() == true);
        std::vector<std::map<std::string, std::string>> rs;
        REQUIRE(mysql.Query("select * from demo", rs) == true);
        CHECK(!rs.empty());
        REQUIRE(mysql.Execute("update demo set age = 21 where id = 1") == true);
    }

    GIVEN("mysql tcp dsn test") {
        uranus::database::MySQL mysql;
        REQUIRE(mysql.Connect("root:ruisi_112@tcp(127.0.0.1:3306)/ruisi") == true);
        REQUIRE(mysql.Ping() == true);
        std::vector<std::map<std::string, std::string>> rs;
        REQUIRE(mysql.Query("select * from demo", rs) == true);
        CHECK(!rs.empty());
        REQUIRE(mysql.Execute("update demo set age = 21 where id = 1") == true);
    }
}