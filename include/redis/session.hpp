#pragma once

#include "fmt/core.h"
#include "packet.hpp"

#include <any>
#include <array>
#include <asio.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace uranus::redis {
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(asio::io_context &ioc) : socket_(ioc) {}

    ~Session() {
        Close();
    }

    auto Connect(std::string_view hostname = "127.0.0.1", const std::uint16_t port = 6379) -> bool {
        asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(hostname.data()), port);
        socket_.connect(endpoint);
        return true;
    }

    auto ReadReply() -> std::any {
        std::string line = readLine();

        switch (line.at(0)) {
            case RespNil:
                return std::nullopt;
            case RespStatus:
            case RespError:
            case RespBigInt:
                return line.substr(1);
            case RespString:
                return readString(line);
            case RespVerbatim:
                return readVerbatim(line);
            case RespInt:
                return readInt(line);
            case RespFloat:
                return readDouble(line);
            case RespBool:
                return readBool(line);
            case RespMap:
                return readMap(line);
            case RespSet:
                return readSet(line);
            case RespArray:
            case RespPush:
                return readSlice(line);
            default:
                return "";
        }
        return "";
    }

    auto Write(const std::vector<std::any> &args) -> std::size_t {
        if (args.empty()) {
            return -1;
        }
        write_.clear();
        write_ = packet_.Write(args);

        return socket_.send(asio::buffer(write_));
    }

    void Close() {
        socket_.close();
    }

private:
    // 去掉 \r\n
    auto readLine() -> std::string {
        std::size_t length = asio::read_until(socket_, buffer_, '\n');

        std::string line(asio::buffers_begin(buffer_.data()), asio::buffers_begin(buffer_.data()) + length - 2);

        buffer_.consume(length);

        fmt::print(stdout, "line {}\n", line);

        return line;
    }

    // -1 是不存在key
    auto readString(std::string &line) -> std::optional<std::string> {
        // 有数据
        if (int number = std::stoi(line.substr(1)); number >= 0) {
            std::string data = readLine();
            if (data.size() == number) {
                return std::make_optional<std::string>(data);
            }
        }
        return std::nullopt;
    }

    // %3 :: map类型，有3个键值对 {key:value}
    auto readMap(std::string &line) -> std::map<std::string, std::any> {
        std::map<std::string, std::any> result{};

        if (int number = std::stoi(line.substr(1)); number > 0) {
            for (auto i = 0; i < number; ++i) {
                auto optKey = ReadReply();
                auto value  = ReadReply();
                if (optKey.has_value() && value.has_value()) {
                    auto key = std::any_cast<std::optional<std::string>>(optKey);
                    if (key.has_value()) {
                        result.try_emplace(key.value(), value);
                    }
                }
            }
        }

        return result;
    }

    auto readSlice(std::string &line) -> std::vector<std::string> {
        std::vector<std::string> result{};
        if (int number = std::stoi(line.substr(1)); number > 0) {
            result.reserve(number);
            for (auto i = 0; i < number; ++i) {
                auto data = ReadReply();
                result.emplace_back(std::any_cast<std::string>(data));
            }
        }
        return result;
    }

    auto readSet(std::string &line) -> std::set<std::string> {
        std::set<std::string> result{};
        if (int number = std::stoi(line.substr(1)); number > 0) {
            for (auto i = 0; i < number; ++i) {
                std::any data = ReadReply();
                result.emplace(std::any_cast<std::string>(data));
            }
        }
        return result;
    }

    // 最大范围不超过std::int64_t
    auto readInt(std::string &line) -> std::int64_t {
        return std::stoll(line.substr(1));
    }

    auto readDouble(std::string &line) -> double {
        return std::stod(line.substr(1));
    }

    auto readBool(std::string &line) -> bool {
        switch (line.at(1)) {
            case 't':
            case '1':
                return true;
            default:
                return false;
        }
    }

    /*
    =15<CR><LF>
    txt:Some string<CR><LF>
    */
    auto readVerbatim(std::string &line) -> std::string {
        int         number = std::stoi(line.substr(1));
        std::string data   = readLine();
        if (data.size() == number) {
            return line.substr(3);
        }
        return "";
    }

    asio::ip::tcp::socket socket_;
    uranus::redis::Packet packet_;
    asio::streambuf       buffer_;  // 读出的数据流
    std::vector<char>     write_;
};
}  // namespace uranus::redis
