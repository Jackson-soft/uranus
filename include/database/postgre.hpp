#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

namespace Uranus::Database
{
// PostgreSQL
class Postgre
{
public:
    Postgre()  = default;
    ~Postgre() = default;

    auto connect(std::string_view dsn) -> bool
    {
        if (dsn.empty())
            return false;
        //conn = std::make_unique<pqxx::connection>(dsn);
        return conn->is_open();
    }

    void insert(const std::string &sql)
    {
        if (sql.empty())
            return;
        //conn->prepare(sql);
        pqxx::work work{*conn};
        //pqxx::result result = work.exec(sql);
        work.commit();
    }

    void close()
    {
        if (conn->is_open()) {
            conn->disconnect();
        }
    }

private:
    std::unique_ptr<pqxx::connection> conn;
};
}  // namespace Uranus::Database
