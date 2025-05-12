#pragma once

#include <string>
#include <string_view>

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

统一资源定位符的标准格式如下：

[协议类型]://[服务器地址]:[端口号]/[资源层级UNIX文件路径][文件名]?[查询]#[片段ID]

统一资源定位符的完整格式如下：

[协议类型]://[访问资源需要的凭证信息]@[服务器地址]:[端口号]/[资源层级UNIX文件路径][文件名]?[查询]#[片段ID]


 uri 解析  ws://localhost:9002/xxx

                    hierarchical part
        ┌───────────────────┴─────────────────────┐
                    authority               path
        ┌───────────────┴───────────────┐┌───┴────┐
  abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1
  └┬┘   └───────┬───────┘ └────┬────┘ └┬┘           └─────────┬─────────┘ └──┬──┘
 scheme  user information     host     port                  query        fragment
*/
namespace uranus::net {
class URI {
public:
    URI() noexcept = default;
    ~URI()         = default;

    auto Parse(std::string_view rawUri) -> bool {
        if (rawUri.empty()) {
            return false;
        }

        std::string_view::size_type position{0};
        if (const auto scheme = rawUri.find("://"); scheme != std::string_view::npos) {
            scheme_  = rawUri.substr(0, scheme);
            position = scheme + 3;
        } else {
            position = 0;
        }

        const auto info    = rawUri.find_last_of('@');
        const auto infoStr = rawUri.substr(position, info - position);
        const auto user    = infoStr.find_first_of(':');
        userName_    = infoStr.substr(0, user);
        password_    = infoStr.substr(user + 1);

        /*
            auto host = addrStr.find_first_of('/');
            host_     = url.substr(info + 1, host - info - 1);
         */

        const auto query = rawUri.find_last_of('?');
        rawQuery_  = rawUri.substr(query + 1);

        const auto fragment = rawUri.find_last_of('#');
        fragment_     = rawUri.substr(fragment + 1);

        return true;
    }

    auto Hostname() -> std::string const & {
        return host_;
    }

    [[nodiscard]] auto Port() const -> std::uint16_t {
        return port_;
    }

    auto Path() -> std::string const & {
        return path_;
    }

    auto Query() -> std::string const & {
        return rawQuery_;
    }

    auto IsAbs() const -> bool {
        return !scheme_.empty();
    }

private:
    std::string scheme_{};
    std::string opaque_{};  // encoded opaque data
    std::string userName_{};
    std::string password_{};
    std::string host_{};         // host or host:port
    std::uint16_t port_{};
    std::string path_{"/"};    // path (relative paths may omit leading slash)
    std::string rawPath_{};      // encoded path hint (see EscapedPath method)
    std::string rawQuery_{};     // encoded query values, without '?'
    std::string fragment_{};     // fragment for references, without '#'
    std::string rawFragment_{};  // encoded fragment hint (see EscapedFragment method)
};
}  // namespace uranus::net
