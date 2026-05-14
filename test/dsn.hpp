#pragma once

#include "database/dsn.hpp"

#include <catch2/catch_test_macros.hpp>
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

    GIVEN("unix socket dsn parse") {
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

SCENARIO("dsn query parameters test") {
    GIVEN("single parameter") {
        const auto           *str{"postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        auto &params = dsn.Parameters();
        REQUIRE(params.size() == 1);
        REQUIRE(params.at("sslmode") == "verify-full");
    }

    GIVEN("multiple parameters") {
        const auto           *str{"mysql://user:password@tcp(127.0.0.1:3306)/"
                                  "dbname?timeout=90s&collation=utf8mb4_unicode_ci&charset=utf8mb4"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.Driver() == "mysql");
        REQUIRE(dsn.Schema() == "dbname");

        auto &params = dsn.Parameters();
        REQUIRE(params.size() == 3);
        REQUIRE(params.at("timeout") == "90s");
        REQUIRE(params.at("collation") == "utf8mb4_unicode_ci");
        REQUIRE(params.at("charset") == "utf8mb4");
    }
}

SCENARIO("dsn edge cases test") {
    GIVEN("empty string") {
        uranus::database::DSN dsn;
        REQUIRE(dsn.Parse("") == false);
    }

    GIVEN("user without password") {
        const auto           *str{"postgres://bob@127.0.0.1:5432/mydb?sslmode=disable"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.User() == "bob");
        REQUIRE(dsn.Password().empty() == true);
        REQUIRE(dsn.Host() == "127.0.0.1");
        REQUIRE(dsn.Port() == std::uint32_t(5432));
        REQUIRE(dsn.Schema() == "mydb");
    }

    GIVEN("dsn without query parameters") {
        const auto           *str{"root:pass@tcp(10.0.0.1:3306)/testdb"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.User() == "root");
        REQUIRE(dsn.Password() == "pass");
        REQUIRE(dsn.Network() == "tcp");
        REQUIRE(dsn.Host() == "10.0.0.1");
        REQUIRE(dsn.Port() == std::uint32_t(3306));
        REQUIRE(dsn.Schema() == "testdb");
        REQUIRE(dsn.Parameters().empty() == true);
    }

    GIVEN("ipv6 host with tcp protocol") {
        const auto           *str{"mysql://user:password@tcp([de:ad:be:ef::ca:fe]:80)/dbname?timeout=90s"};
        uranus::database::DSN dsn;

        REQUIRE(dsn.Parse(str) == true);

        REQUIRE(dsn.Driver() == "mysql");
        REQUIRE(dsn.User() == "user");
        REQUIRE(dsn.Password() == "password");
        REQUIRE(dsn.Network() == "tcp");
        REQUIRE(dsn.Host() == "[de:ad:be:ef::ca:fe]");
        REQUIRE(dsn.Port() == std::uint32_t(80));
        REQUIRE(dsn.Schema() == "dbname");
    }

    GIVEN("missing closing parenthesis") {
        const auto           *str{"root:pass@tcp(127.0.0.1:3306/testdb"};
        uranus::database::DSN dsn;
        REQUIRE(dsn.Parse(str) == false);
    }
}
