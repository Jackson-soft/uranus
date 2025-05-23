#pragma once

#include <string_view>

// database interface
namespace uranus::database {
class Database {
public:
    Database()          = default;
    virtual ~Database() = default;

    virtual auto Connect(std::string_view dsn) -> bool = 0;
    virtual auto Close() -> bool                       = 0;
};
}  // namespace uranus::database
