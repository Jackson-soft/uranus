#pragma once

#include <charconv>
#include <map>
#include <string>
#include <string_view>

/*
  Data Source Name => <driver>://[username[:password]@][protocol[(host:port)]]/dbname[?param1=value1&...&paramN=valueN]
  mysql://user:password@tcp([de:ad:be:ef::ca:fe]:80)/dbname?timeout=90s&collation=utf8mb4_unicode_ci
  postgres://bob:secret@1.2.3.4:5432/mydb?sslmode=verify-full
*/
namespace uranus::database {
class DSN {
public:
    DSN()  = default;
    ~DSN() = default;

    auto Parse(std::string_view url) -> bool {
        if (url.empty()) {
            return false;
        }

        std::string_view::size_type position{0};
        if (const auto driver = url.find("://"); driver != std::string_view::npos) {
            driver_  = url.substr(0, driver);
            position = driver + 3;
        } else {
            position = 0;
        }

        auto info    = url.find_last_of('@');
        auto infoStr = url.substr(position, info - position);
        auto user    = infoStr.find_first_of(':');
        user_        = infoStr.substr(0, user);
        password_    = infoStr.substr(user + 1);

        // pgsql://user:pass@tcp(localhost:5555)/dbname?aa=bb
        const auto address = url.find_last_of('?');
        auto addrStr = url.substr(info + 1, address - info - 1);

        std::string_view::size_type schema{0};
        if (auto protocol = addrStr.find_first_of('('); protocol != std::string_view::npos) {
            // 存在协议名称
            protocol_ = addrStr.substr(0, protocol);

            std::string_view::size_type hostEnd{0};
            if (protocol_ == "unix") {
                hostEnd = addrStr.find_last_of(')');
                host_   = addrStr.substr(protocol + 1, hostEnd - protocol - 1);
                schema  = hostEnd + 2;
            } else {
                hostEnd = addrStr.find_last_of(':');
                host_   = addrStr.substr(protocol + 1, hostEnd - protocol - 1);

                auto port    = addrStr.find_last_of(')');
                auto portStr = addrStr.substr(hostEnd + 1, port - hostEnd - 1);
                if (auto [ptr, ec] = std::from_chars(portStr.begin(), portStr.end(), port_); ptr != portStr.end()) {
                    return false;
                }
                schema = port + 2;
            }
        } else {
            auto host = addrStr.find_last_of(':');
            host_     = addrStr.substr(0, host);

            auto port    = addrStr.find_last_of('/');
            auto portStr = addrStr.substr(host + 1, port - host - 1);
            if (auto [ptr, ec] = std::from_chars(portStr.begin(), portStr.end(), port_); ptr != portStr.end()) {
                return false;
            }

            schema = port + 1;
        }

        schema_ = addrStr.substr(schema);

        auto paramStr = url.substr(address + 1);

        return true;
    }

    auto Driver() -> std::string & {
        return driver_;
    }

    auto User() -> std::string & {
        return user_;
    }

    auto Password() -> std::string & {
        return password_;
    }

    auto Network() -> std::string & {
        return protocol_;
    }

    auto Host() -> std::string & {
        return host_;
    }

    [[nodiscard]] auto Port() const -> std::uint32_t {
        return port_;
    }

    auto Schema() -> std::string & {
        return schema_;
    }

private:
    std::string                        driver_{};
    std::string                        user_{};
    std::string                        password_{};
    std::string                        protocol_{};
    std::string                        host_{};
    std::uint32_t                      port_{};
    std::string                        schema_{};
    std::map<std::string, std::string> parameters_{};
};
}  // namespace uranus::database
