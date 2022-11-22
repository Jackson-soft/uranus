#pragma once

#include <string>
#include <string_view>

namespace uranus::utils {
/*
    https://www.rfc-editor.org/rfc/rfc3986

                        hierarchical part
        ┌───────────────────┴─────────────────────┐
                    authority               path
        ┌───────────────┴───────────────┐┌───┴────┐
  abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1
  └┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └─────────┬─────────┘ └──┬──┘
scheme  user information     host     port                  query         fragment

  urn:example:mammal:monotreme:echidna
  └┬┘ └──────────────┬───────────────┘
scheme              path
*/
class URL {
public:
    URL()  = default;
    ~URL() = default;

    auto Parse(std::string_view url) -> bool {
        if (url.empty()) {
            return false;
        }

        std::string_view::size_type position{0};
        auto                        scheme = url.find("://");
        if (scheme != std::string_view::npos) {
            scheme_  = url.substr(0, scheme);
            position = scheme + 3;
        } else {
            position = 0;
        }

        auto info    = url.find_last_of('@');
        auto infoStr = url.substr(position, info - position);
        auto user    = infoStr.find_first_of(':');
        userName_    = infoStr.substr(0, user);
        password_    = infoStr.substr(user + 1);

        /*
                auto host = addrStr.find_first_of('/');
                host_     = url.substr(info + 1, host - info - 1);
                */

        auto query = url.find_last_of('?');
        rawQuery_  = url.substr(query + 1);

        auto fragment = url.find_last_of('#');
        fragment_     = url.substr(fragment + 1);

        return true;
    }

private:
    std::string scheme_;
    std::string opaque_;  // encoded opaque data
    std::string userName_;
    std::string password_;
    std::string host_;         // host or host:port
    std::string path_;         // path (relative paths may omit leading slash)
    std::string rawPath_;      // encoded path hint (see EscapedPath method)
    std::string rawQuery_;     // encoded query values, without '?'
    std::string fragment_;     // fragment for references, without '#'
    std::string rawFragment_;  // encoded fragment hint (see EscapedFragment method)
};
}  // namespace uranus::utils
