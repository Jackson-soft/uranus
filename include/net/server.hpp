#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <thread>

#include "net/ioPool.hpp"

namespace uranus::net
{
class server
{
public:
    explicit server(std::size_t size = std::thread::hardware_concurrency()): iocPool_(size) {}

    ~server() { iocPool_.stop(); }

    auto listen(std::uint16_t port, std::string_view host = "0.0.0.0") -> bool
    {
        if (port <= 0)
            return false;

        auto address = boost::asio::ip::make_address(host);
        auto ep      = boost::asio::ip::tcp::endpoint(address, port);
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

    void run() { iocPool_.run(); }

private:
    void fail(boost::system::error_code ec, char const *what) { std::cerr << what << ": " << ec.message() << "\n"; }

    void doAccept() {}

    ioPool iocPool_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_{};
    std::shared_ptr<boost::asio::ip::tcp::socket> socket{};
};
}  // namespace uranus::net
