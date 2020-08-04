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
  └┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └─────────┬─────────┘ └──┬──┘
scheme  user information     host     port                  query         fragment

*/
namespace Uranus::WebSocket
{
class URI
{
public:
    URI()  = default;
    ~URI() = default;

    auto parse(std::string_view rawurl) -> bool
    {
        if (rawurl.empty())
            return false;
        return false;
    }

    auto hostname() -> std::string & { return host; }

    auto port() -> std::uint16_t { return port_; }

    auto isAbs() -> bool { return !scheme.empty(); }

private:
    std::string scheme;
    std::string host;
    std::uint16_t port_;
    std::string path;
    std::string query;
    std::string fragment;
};
}  // namespace Uranus::WebSocket
