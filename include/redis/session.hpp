#pragma once

#include "boost/asio/buffer.hpp"
#include "boost/asio/buffers_iterator.hpp"
#include "boost/asio/read.hpp"
#include "boost/asio/read_until.hpp"
#include "boost/asio/streambuf.hpp"
#include "packet.hpp"

#include <any>
#include <array>
#include <boost/asio.hpp>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uranus::redis {
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::io_context &ioc) : socket_(ioc) {}

    ~Session() = default;

    auto Connect(std::string_view hostname = "127.0.0.1", const std::uint16_t port = 6379) -> bool {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(hostname.data()), port);
        socket_.connect(endpoint);
        return true;
    }

    auto ReadReply() -> std::string {
        auto line = readLine();

        if (line.starts_with(RespStatus)) {
            return line.substr(1);
        }
        if (line.starts_with(RespString)) {
            auto number = std::stoi(line.substr(1));
            return readLen(number + 2).substr(0, number);
        }
        if (line.starts_with(RespMap)) {
            line = readLine();
            // return readMap(line);
            return line;
        }

        return "";
    }

    template<typename... Args>
    auto Write(Args... args) -> std::size_t {
        if (sizeof...(args) == 0) {
            return -1;
        }
        auto data = packet_.Write(args...);

        return socket_.send(boost::asio::buffer(data));
    }

private:
    // 去掉 \r\n
    auto readLine() -> std::string {
        std::string buffer;

        auto len = boost::asio::read_until(socket_, boost::asio::dynamic_buffer(buffer), '\n');
        return buffer.substr(0, len - 2);
    }

    auto readLen(int size) -> std::string {
        std::string data;
        auto        len = boost::asio::read(socket_, boost::asio::buffer(data, size));
        return data.substr(0, size);
    }

    auto readMap(std::string &line) -> std::string {
        auto                               number = std::stoi(line.substr(1));
        std::map<std::string, std::string> result;
        if (number > 0) {
            for (auto i = 0; i < number; ++i) {
                auto key   = ReadReply();
                auto value = ReadReply();
                result.try_emplace(key, value);
            }
        }

        return "";
    }

    boost::asio::ip::tcp::socket socket_;
    uranus::redis::Packet        packet_;
};
}  // namespace uranus::redis
