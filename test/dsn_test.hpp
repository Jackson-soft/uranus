#pragma once

#include "database/dsn.hpp"

#include <catch2/catch.hpp>
#include <cstdint>

SCENARIO("postgresql dsn test") {
    GIVEN("tcp dsn parse") {
        const auto           *str{"postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.Driver() == "postgres");

        REQUIRE(dsn.User() == "bob");
        REQUIRE(dsn.Password() == "secret");
        REQUIRE(dsn.Host() == "1.2.3.4");
        REQUIRE(dsn.Port() == std::uint32_t(5432));
        REQUIRE(dsn.Schema() == "mydb");
    }
}

SCENARIO("mysql dsn test") {
    GIVEN("tcp dsn parse") {
        const auto           *str{"root:ruisi_112@tcp(127.0.0.1:3306)/ruisi"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.Driver().empty() == true);

        REQUIRE(dsn.User() == "root");
        REQUIRE(dsn.Password() == "ruisi_112");
        REQUIRE(dsn.Host() == "127.0.0.1");
        REQUIRE(dsn.Port() == std::uint32_t(3306));
        REQUIRE(dsn.Schema() == "ruisi");
    }

    GIVEN("tcp dsn parse") {
        const auto           *str{"root:ruisi_112@unix(/var/run/mysqld/mysqld.sock)/ruisi"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.Driver().empty() == true);

        REQUIRE(dsn.User() == "root");
        REQUIRE(dsn.Password() == "ruisi_112");
        REQUIRE(dsn.Network() == "unix");
        REQUIRE(dsn.Host() == "/var/run/mysqld/mysqld.sock");
        REQUIRE(dsn.Port() == 0);
        REQUIRE(dsn.Schema() == "ruisi");
    }
}
