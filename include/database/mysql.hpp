#pragma once

#include "database/database.hpp"
#include "database/dsn.hpp"

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <mysql.h>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace uranus::database {
const std::uint32_t mysqlPort = 3306;
class MySQL : public DataBase {
public:
    MySQL() : mysql_(std::make_unique<MYSQL>()) {}

    ~MySQL() {
        auto *ptr = mysql_.release();
        ::mysql_close(ptr);
    }

    auto Connect(std::string_view dsnStr) -> bool override {
        if (dsnStr.empty()) {
            return false;
        }

        uranus::database::DSN dsn;
        if (!dsn.Parse(dsnStr)) {
            return false;
        }

        return Connect(dsn.User(), dsn.Password(), dsn.Schema(), dsn.Host(), dsn.Port());
    }

    auto Connect(std::string_view user,
                 std::string_view pwd,
                 std::string_view schema,
                 std::string_view host = "127.0.0.1",
                 std::uint32_t    port = mysqlPort) -> bool {
        if ((::mysql_init(mysql_.get())) == nullptr) {
            return false;
        }

        // Call mysql_options() after mysql_init() and before mysql_connect() or mysql_real_connect().
        std::uint32_t arg{0};
        if (::mysql_options(mysql_.get(), MYSQL_OPT_RECONNECT, &arg) != 0) {
            return false;
        }
        if (::mysql_options(mysql_.get(), MYSQL_INIT_COMMAND, "SET autocommit=1") != 0) {
            return false;
        }

        return ::mysql_real_connect(mysql_.get(), host.data(), user.data(), pwd.data(), schema.data(), port, nullptr, 0)
            != nullptr;
    }

    auto Ping() -> bool {
        return ::mysql_ping(mysql_.get()) == 0;
    }

    // select
    auto Query(std::string_view sql, std::vector<std::map<std::string, std::string>> &rs) -> bool {
        if (sql.empty()) {
            return false;
        }
        if (::mysql_real_query(mysql_.get(), sql.data(), sql.size()) != 0) {
            return false;
        }

        auto *result = ::mysql_store_result(mysql_.get());
        if (result == nullptr) {
            return false;
        }

        auto rowNum = ::mysql_num_rows(result);  // 数据行数
        auto field  = std::span(::mysql_fetch_fields(result), ::mysql_num_fields(result));

        rs.clear();
        rs.reserve(rowNum);

        MYSQL_ROW row = nullptr;
        while ((row = ::mysql_fetch_row(result)) != nullptr) {
            std::map<std::string, std::string> rr;
            for (auto i = 0; i < field.size(); ++i) {
                rr.emplace(field[i].name, row[i]);
            }
            rs.emplace_back(rr);
        }

        ::mysql_free_result(result);
        return true;
    }

    // stmt

    // insert update
    auto Execute(std::string_view sql) -> bool {
        if (sql.empty()) {
            return false;
        }
        if (::mysql_real_query(mysql_.get(), sql.data(), sql.size()) != 0) {
            return false;
        }
        return ::mysql_affected_rows(mysql_.get()) >= 0;
    }

    auto Error() -> std::pair<std::uint32_t, std::string> {
        return {::mysql_errno(mysql_.get()), ::mysql_error(mysql_.get())};
    }

    auto Close() -> bool override {
        return true;
    }

private:
    std::unique_ptr<MYSQL> mysql_;
};
}  // namespace uranus::database
