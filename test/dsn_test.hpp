#pragma once

#include "database/dsn.hpp"

#include <cstdint>
#include <doctest/doctest.h>

TEST_SUITE("postgresql dsn test") {
    TEST_CASE("tcp dsn parse") {
        const auto           *str{"postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full"};
        uranus::database::DSN dsn;

        CHECK_EQ(dsn.Parse(str), true);

        CHECK_EQ(dsn.Driver(), "postgres");

        CHECK_EQ(dsn.User(), "bob");
        CHECK_EQ(dsn.Password(), "secret");
        CHECK_EQ(dsn.Host(), "1.2.3.4");
        CHECK_EQ(dsn.Port(), std::uint32_t(5432));
        CHECK_EQ(dsn.Schema(), "mydb");
    }
}

TEST_SUITE("mysql dsn test") {
    TEST_CASE("tcp dsn parse") {
        const auto           *str{"root:ruisi_112@tcp(127.0.0.1:3306)/ruisi"};
        uranus::database::DSN dsn;

        CHECK_EQ(dsn.Parse(str), true);

        CHECK_EQ(dsn.Driver(), "");

        CHECK_EQ(dsn.User(), "root");
        CHECK_EQ(dsn.Password(), "ruisi_112");
        CHECK_EQ(dsn.Host(), "127.0.0.1");
        CHECK_EQ(dsn.Port(), std::uint32_t(3306));
        CHECK_EQ(dsn.Schema(), "ruisi");
    }

    TEST_CASE("tcp dsn parse") {
        const auto           *str{"root:ruisi_112@unix(/var/run/mysqld/mysqld.sock)/ruisi"};
        uranus::database::DSN dsn;

        CHECK_EQ(dsn.Parse(str), true);

        CHECK_EQ(dsn.Driver(), "");

        CHECK_EQ(dsn.User(), "root");
        CHECK_EQ(dsn.Password(), "ruisi_112");
        CHECK_EQ(dsn.Network(),"unix");
        CHECK_EQ(dsn.Host(), "/var/run/mysqld/mysqld.sock");
        CHECK_EQ(dsn.Port(), 0);
        CHECK_EQ(dsn.Schema(), "ruisi");
    }
}