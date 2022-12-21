#pragma once

#include "boost/asio/buffer.hpp"
#include "boost/asio/read_until.hpp"
#include "packet.hpp"

#include <any>
#include <boost/asio.hpp>
#include <cstddef>
#include <memory>
#include <string>
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

    auto Read() -> int {
        std::string buffer;

        auto len = boost::asio::read_until(socket_, boost::asio::dynamic_buffer(buffer), "\n");
        return 0;
    }

    template<typename... Args>
    auto Write(Args... args) -> std::size_t {
        if (sizeof...(args) == 0) {
            return -1;
        }
        std::vector<std::byte> data = packet_.Write(args...);

        return socket_.send(boost::asio::buffer(data));
    }

private:
    boost::asio::ip::tcp::socket socket_;
    uranus::redis::Packet        packet_;
};
}  // namespace uranus::redis
