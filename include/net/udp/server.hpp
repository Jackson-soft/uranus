#pragma once

#include "boost/asio/buffer.hpp"

#include <array>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/udp.hpp>
// #include <boost/asio/steady_timer.hpp>
#include "utils/log.hpp"

#include <algorithm>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uranus::udp {
class Server {
public:
    explicit Server(boost::asio::io_context &ioc) : socket_(std::make_shared<boost::asio::ip::udp::socket>(ioc)) {}

    void Listen(const std::uint16_t port, std::string_view host = "0.0.0.0") {
        auto ep = boost::asio::ip::udp::endpoint(boost::asio::ip::make_address(host), port);
        socket_->open(ep.protocol());
        socket_->non_blocking(true);
        socket_->bind(ep);
    }

    void Run() {  // boost::asio::co_spawn(socket_->get_executor(), reader(), boost::asio::detached);
        reader();
    }

private:
    // auto reader() -> boost::asio::awaitable<void>
    void reader() {
        while (true) {
            /*
            auto bytes = co_await socket_->async_receive_from(
                boost::asio::buffer(buffer_), remote_, boost::asio::use_awaitable);
 */
            auto bytes = socket_->receive_from(boost::asio::buffer(buffer_), remote_);

            std::vector<std::byte> tmp{};
            tmp.reserve(bytes);
            std::copy_n(buffer_.begin(), bytes, std::back_inserter(tmp));
            uranus::utils::LogHelper::Instance().info("receive data bytes: {}, data size: {}", bytes, tmp.size());

            // co_await socket_.async_send_to(boost::asio::buffer(buffer_, bytes), remote_, boost::asio::use_awaitable);
        }
    }

    std::shared_ptr<boost::asio::ip::udp::socket> socket_;
    boost::asio::ip::udp::endpoint                remote_;
    std::array<std::byte, 2048>                   buffer_{};
};
}  // namespace uranus::udp
