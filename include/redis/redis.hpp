#pragma once

#include <memory>
#include <string_view>
#include <utility>

extern "C" {
#include <hiredis/hiredis.h>
}

namespace uranus::redis {
class Redis {
public:
    Redis()  = default;
    ~Redis() = default;

    auto Dial(std::string_view hostname = "127.0.0.1", int port = 6379) -> bool
    {
        auto *ctx = ::redisConnectNonBlock(hostname.data(), port);
        if (ctx == nullptr || (ctx->err != 0)) {
            return false;
        }
        ctx_ = std::make_shared<::redisContext>(*ctx);
        return true;
    }

    auto Ping() -> int
    {
        auto *reply = static_cast<::redisReply *>(::redisCommand(ctx_.get(), "PING"));
        if (reply == nullptr) {
            return 1;
        }

        ::freeReplyObject(reply);
        return 0;
    }

    auto Set(std::string_view key, std::string_view value) -> int
    {
        if (key.empty() || value.empty()) {
            return 1;
        }
        auto *reply = redisCommand(ctx_.get(), "SET %s %s", key, value);
        return 0;
    }

    auto Get(std::string_view key) -> std::string_view
    {
        return "";
    }

private:
    std::shared_ptr<::redisContext> ctx_;
};
}  // namespace uranus::cache
