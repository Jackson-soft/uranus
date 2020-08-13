#pragma once

#include <pqxx/pqxx>

namespace Uranus::Database
{
// PostgreSQL
class Postgre
{
public:
    Postgre()  = default;
    ~Postgre() = default;

private:
    pqxx::connection conn;
};
}  // namespace Uranus::Database