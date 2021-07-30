#pragma once

#include <cstdint>
#include <string>
#include <string_view>

// uri 解析  ws://localhost:9002/xxx

/*
                    hierarchical part
        ┌───────────────────┴─────────────────────┐
                    authority               path
        ┌───────────────┴───────────────┐┌───┴────┐
  abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1
  └┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └─────────┬─────────┘
└──┬──┘ scheme  user information     host     port                  query
fragment

*/
namespace uranus::net {
class URI {
public:
    URI() noexcept = default;
    ~URI()         = default;

    auto parse(std::string_view rawuri) -> bool
    {
        if (rawuri.empty())
            return false;
        return false;
    }

    auto hostname() -> std::string const &
    {
        return host_;
    }

    [[nodiscard]] auto port() const -> std::uint16_t
    {
        return port_;
    }

    auto path() -> std::string const &
    {
        return path_;
    }

    auto query() -> std::string const &
    {
        return query_;
    }

    auto isAbs() -> bool
    {
        return !scheme_.empty();
    }

private:
    std::string   scheme_;
    std::string   host_;
    std::uint16_t port_;
    std::string   path_{"/"};
    std::string   query_;
    std::string   fragment_;
};
}  // namespace uranus::net