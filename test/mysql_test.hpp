#pragma once

#include "database/mysql.hpp"
#include "database/pool.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

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

	GIVEN("mysql tcp dsn test") {
		uranus::database::MySQL mysql;
		auto                    dsn = "wuxiangwei:qM8<rV0*xQ2@tO4(@tcp(172.28.67.193:7033)/waha_roomcenter";
		REQUIRE(mysql.Connect(dsn) == true);
		REQUIRE(mysql.Ping() == true);
		std::vector<std::map<std::string, std::string>> rs;
		REQUIRE(mysql.Query("select scenes_id,file_name from room_scenes_config where fstate = 1", rs) == true);
		CHECK(!rs.empty());
		fmt::print("Hello, world!\n");
		for (auto &it : rs) {
			for (const auto &[key, value] : it) {
				fmt::print("key: {}, value: {}\n", key, value);
			}
		}
	}
}
