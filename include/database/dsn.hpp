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

        auto remaining = url;

        // 解析 driver (scheme)
        if (const auto pos = remaining.find("://"); pos != std::string_view::npos) {
            driver_   = remaining.substr(0, pos);
            remaining = remaining.substr(pos + 3);
        }

        // 解析 user:password@
        if (const auto at = remaining.find_last_of('@'); at != std::string_view::npos) {
            auto userInfo = remaining.substr(0, at);
            remaining     = remaining.substr(at + 1);

            if (const auto colon = userInfo.find_first_of(':'); colon != std::string_view::npos) {
                user_     = userInfo.substr(0, colon);
                password_ = userInfo.substr(colon + 1);
            } else {
                user_ = userInfo;
            }
        }

        // 分离查询参数
        if (const auto q = remaining.find('?'); q != std::string_view::npos) {
            parseParams(remaining.substr(q + 1));
            remaining = remaining.substr(0, q);
        }

        // 解析 protocol(host:port)/schema 或 host:port/schema
        if (const auto paren = remaining.find('('); paren != std::string_view::npos) {
            protocol_ = remaining.substr(0, paren);

            auto closeParen = remaining.find(')');
            if (closeParen == std::string_view::npos) {
                return false;
            }

            auto address = remaining.substr(paren + 1, closeParen - paren - 1);

            if (protocol_ == "unix") {
                host_ = address;
            } else if (!parseHostPort(address)) {
                return false;
            }

            if (closeParen + 2 <= remaining.size()) {
                schema_ = remaining.substr(closeParen + 2);
            }
        } else {
            auto slash = remaining.find_last_of('/');
            if (slash == std::string_view::npos) {
                return false;
            }

            schema_ = remaining.substr(slash + 1);
            if (!parseHostPort(remaining.substr(0, slash))) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto Driver() const -> const std::string & {
        return driver_;
    }

    [[nodiscard]] auto User() const -> const std::string & {
        return user_;
    }

    [[nodiscard]] auto Password() const -> const std::string & {
        return password_;
    }

    [[nodiscard]] auto Network() const -> const std::string & {
        return protocol_;
    }

    [[nodiscard]] auto Host() const -> const std::string & {
        return host_;
    }

    [[nodiscard]] auto Port() const -> std::uint32_t {
        return port_;
    }

    [[nodiscard]] auto Schema() const -> const std::string & {
        return schema_;
    }

    [[nodiscard]] auto Parameters() const -> const std::map<std::string, std::string> & {
        return parameters_;
    }

private:
    auto parseHostPort(std::string_view address) -> bool {
        auto colon = address.find_last_of(':');
        if (colon == std::string_view::npos) {
            host_ = address;
            return true;
        }

        host_          = address.substr(0, colon);
        auto portStr   = address.substr(colon + 1);
        auto [ptr, ec] = std::from_chars(portStr.data(), portStr.data() + portStr.size(), port_);
        return ptr == portStr.data() + portStr.size() && ec == std::errc{};
    }

    void parseParams(std::string_view params) {
        while (!params.empty()) {
            auto amp  = params.find('&');
            auto pair = (amp != std::string_view::npos) ? params.substr(0, amp) : params;

            if (auto eq = pair.find('='); eq != std::string_view::npos) {
                parameters_.emplace(pair.substr(0, eq), pair.substr(eq + 1));
            }

            if (amp == std::string_view::npos) {
                break;
            }
            params = params.substr(amp + 1);
        }
    }

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
