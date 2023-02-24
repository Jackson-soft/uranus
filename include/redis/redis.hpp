#pragma once

#include "fmt/core.h"
#include "redis/commands.hpp"
#include "redis/options.hpp"
#include "redis/session.hpp"

#include <any>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace uranus::redis {
class Client : public CmdAble {
public:
    Client() : session_(ioContext_) {}

    ~Client() override {
        session_.Close();
    }

    auto Dial(std::string_view hostname = "127.0.0.1", const std::uint16_t port = 6379) -> bool {
        if (hostname.empty()) {
            return false;
        }

        session_.Connect(hostname, port);

        return resp3();
    }

    auto Ping() -> bool override {
        session_.Write({"PING"});
        session_.ReadReply();
        return true;
    }

    auto Set(std::string_view key, const std::any &value, std::unique_ptr<SetArgs> args = nullptr) -> bool override {
        if (key.empty()) {
            return false;
        }

        std::vector<std::any> cmd{"set", key.data(), value};
        if (args) {
            if (args->EX_ > 0) {
                cmd.emplace_back("ex");
                cmd.emplace_back(args->EX_);
            } else if (args->PX_ > 0) {
                cmd.emplace_back("px");
                cmd.emplace_back(args->PX_);
            } else if (args->EXAT_ > 0) {
                cmd.emplace_back("exat");
                cmd.emplace_back(args->EXAT_);
            } else if (args->PXAT_ > 0) {
                cmd.emplace_back("pxat");
                cmd.emplace_back(args->PXAT_);
            } else if (args->NX_) {
                cmd.emplace_back("nx");
            } else if (args->XX_) {
                cmd.emplace_back("xx");
            } else if (args->KeepTTL_) {
                cmd.emplace_back("keepttl");
            }
        }

        if (session_.Write(cmd) == 0) {
            return false;
        }
        session_.ReadReply();
        return true;
    }

    auto Get(std::string_view key) -> std::optional<std::string> override {
        if (key.empty()) {
            return std::nullopt;
        }
        std::vector<std::any> cmd{"get", key.data()};
        if (session_.Write(cmd) == 0) {
            return std::nullopt;
        }
        auto result = session_.ReadReply();
        if (result.has_value()) {
            fmt::print(stdout, "get value: {}\n", std::any_cast<std::optional<std::string>>(result).value());
            return std::any_cast<std::optional<std::string>>(result);
        }
        return std::nullopt;
    }

    auto Del(std::string_view key) -> std::int64_t override {
        if (key.empty()) {
            return 0;
        }
        std::vector<std::any> cmd{"del", key.data()};

        if (session_.Write(cmd) == 0) {
            return -1;
        }
        auto result = session_.ReadReply();
        if (result.has_value()) {
            fmt::print(stdout, "delete result: {}\n", std::any_cast<std::optional<std::int64_t>>(result).value());
        }
        return -1;
    }

private:
    /*
     * 切换到 RESP3 协议
     *
     * https://redis.io/commands/hello/
     */
    auto resp3() -> bool {
        session_.Write({"HELLO", 3});
        session_.ReadReply();
        return true;
    }

    asio::io_context ioContext_;
    Session          session_;
    Options          options_;
};
}  // namespace uranus::redis
