#pragma once

#include "database/dsn.hpp"

#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

// PostgreSQL
namespace uranus::database {
const std::uint32_t pgPort = 5432;
class Postgre {
public:
    Postgre()  = default;
    ~Postgre() = default;

    auto Connect(std::string_view url) -> bool {
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

    void Insert(std::string_view sql) {
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
    std::unique_ptr<pqxx::connection> conn_;
};
}  // namespace uranus::database
