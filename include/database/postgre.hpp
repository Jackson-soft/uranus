#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

namespace uranus::database {
// PostgreSQL
class Postgre {
public:
    Postgre()  = default;
    ~Postgre() = default;

    auto connect(std::string_view dsn) -> bool
    {
        if (dsn.empty()) {
            return false;
        }
        // conn_ = std::make_unique<pqxx::connection>(dsn);
        return conn_->is_open();
    }

    void insert(const std::string &sql)
    {
        if (sql.empty()) {
            return;
        }
        // conn_->prepare(sql);
        pqxx::work work{*conn_};
        // pqxx::result result = work.exec(sql);
        work.commit();
    }

    void close()
    {
        if (conn_->is_open()) {
            conn_->close();
        }
    }

private:
    std::unique_ptr<pqxx::connection> conn_;
};
}  // namespace uranus::database
