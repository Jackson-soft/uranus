#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>

#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include "net/io_pool.hpp"
#include "net/tcp/connection.hpp"

namespace uranus::tcp
{
class Server
{
public:
    explicit Server(std::uint32_t size = std::thread::hardware_concurrency())
        : iocPool_(size), acceptor_(std::make_shared<boost::asio::ip::tcp::acceptor>(iocPool_.getIOContext()))
    {
    }

    ~Server() { iocPool_.stop(); }

    auto listen(const std::uint16_t port, std::string_view host = "0.0.0.0") -> bool
    {
        if (port <= 0)
            return false;

        auto ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host), port);
        return listen(ep);
    }

    auto listen(boost::asio::ip::tcp::endpoint const &endpoint) -> bool
    {
        boost::system::error_code ec;

        acceptor_->open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return false;
        }

        // Allow address reuse
        acceptor_->set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return false;
        }

        // Bind to the server address
        acceptor_->bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return false;
        }

        // Start listening for connections
        acceptor_->listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return false;
        }

        return true;
    }

    void run()
    {
        boost::asio::co_spawn(acceptor_->get_executor(), doAccept(), boost::asio::detached);
        iocPool_.run();
    }

private:
    void fail(boost::system::error_code ec, std::string_view what)
    {
        std::cerr << what << ": " << ec.message() << std::endl;
    }

    auto doAccept() -> boost::asio::awaitable<void>
    {
        while (true) {
            auto socket = co_await acceptor_->async_accept(boost::asio::use_awaitable);
            // auto ep     = socket.remote_endpoint();
            auto conn = std::make_shared<uranus::tcp::Connection>(std::move(socket));
            conn->run();
        }
    }

    uranus::net::IoPool iocPool_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
};
}  // namespace uranus::tcp
