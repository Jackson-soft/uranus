#pragma once

#include "database/dsn.hpp"

#include <memory>
#include <pqxx/pqxx>
#include <string_view>

// PostgreSQL
namespace uranus::database {
constexpr std::uint32_t pgPort = 5432;

class Postgre {
public:
    Postgre()  = default;
    ~Postgre() = default;

    auto Connect(const std::string_view url) const -> bool {
        if (url.empty()) {
            return false;
        }

        uranus::database::DSN dsn;
        if (!dsn.Parse(url)) {
            return false;
        }
        // conn_ = std::make_unique<pqxx::connection>(dsn);
        return conn_->is_open();
    }

    void Insert(const std::string_view sql) const {
        if (sql.empty()) {
            return;
        }
        // conn_->prepare(sql);
        pqxx::work work{*conn_};
        // pqxx::result result = work.exec(sql);
        work.commit();
    }

    void Close() {
        if (conn_->is_open()) {
            conn_->close();
        }
    }

private:
    std::unique_ptr<pqxx::connection> conn_{std::make_unique<pqxx::connection>()};
};
}  // namespace uranus::database
