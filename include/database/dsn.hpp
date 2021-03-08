#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>

namespace uranus::database
{
// Data Source Name
// <driver>://[username[:password]@][protocol[(host:port)]]/dbname[?param1=value1&...&paramN=valueN]
// mysql://user:password@tcp([de:ad:be:ef::ca:fe]:80)/dbname?timeout=90s&collation=utf8mb4_unicode_ci
// postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full
class DSN
{
public:
    DSN()  = default;
    ~DSN() = default;

    auto parse(std::string_view url) -> bool
    {
        if (url.empty())
            return false;

        std::size_t driver = url.find_first_of("://");
        driver_            = url.substr(0, driver);

        std::size_t info = url.find_last_of("@");
        auto infoStr     = url.substr(driver + 3, info - driver - 3);
        std::size_t user = infoStr.find_first_of(":");
        user_            = infoStr.substr(0, user);
        password_        = infoStr.substr(user + 1);

        std::size_t address = url.find_last_of("?");
        auto addrStr        = url.substr(info + 1, address - info - 1);
        std::size_t host    = addrStr.find_last_of(":");
        host_               = addrStr.substr(0, host);
        std::size_t port    = addrStr.find_last_of("/");
        auto portStr        = addrStr.substr(host + 1, port - host - 1);
        if (auto [p, ec] = std::from_chars(portStr.begin(), portStr.end(), port_); ec == std::errc()) {
            return false;
        }

        dbName_ = addrStr.substr(port + 1);

        auto paramStr = url.substr(address + 1);

        return true;
    }

    auto driver() -> std::string & { return driver_; }

    auto user() -> std::string & { return user_; }

    auto password() -> std::string & { return password_; }

    auto host() -> std::string & { return host_; }

    auto port() -> std::uint16_t { return port_; }

    auto dbName() -> std::string & { return dbName_; }

private:
    std::string driver_;
    std::string user_;
    std::string password_;
    std::string host_;
    std::uint16_t port_;
    std::string dbName_;
    std::map<std::string, std::string> parameters_;
};
}  // namespace uranus::database
