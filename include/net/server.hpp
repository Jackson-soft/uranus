#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <thread>

#include "io_pool.hpp"

namespace Uranus::Net
{
class Server: boost::asio::coroutine
{
public:
    explicit Server(std::size_t size = std::thread::hardware_concurrency()): ioPool(size) {}

    ~Server() { ioPool.stop(); }

    auto listen(std::uint16_t port, std::string_view host = "0.0.0.0") -> bool
    {
        if (port <= 0)
            return false;

        auto address = boost::asio::ip::make_address(host);
        return true;
    }

    auto listen(boost::asio::ip::tcp::endpoint const &endpoint) -> bool
    {
        boost::system::error_code ec;

        acceptor->open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return false;
        }

        // Allow address reuse
        acceptor->set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return false;
        }

        // Bind to the server address
        acceptor->bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return false;
        }

        // Start listening for connections
        acceptor->listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return false;
        }

        return true;
    }

    void run()
    {
        doAccept();
        ioPool.run();
    }

private:
    void fail(boost::system::error_code ec, char const *what) { std::cerr << what << ": " << ec.message() << "\n"; }

    void doAccept()
    {
        /*
        acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
            }
            doAccept();
        });
        */
    }

#include <boost/asio/yield.hpp>

#include <boost/asio/unyield.hpp>

    IOPool ioPool;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
};
}  // namespace Uranus::Net