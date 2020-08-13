#pragma once

#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "connection.hpp"
#include "io_pool.hpp"
#include "log.hpp"

namespace Uranus::WebSocket
{

using handler = std::function<void()>;

class Server
{
public:
    explicit Server(std::size_t size = std::thread::hardware_concurrency())
        : ioPool(size), acceptor(ioPool.getIOContext())
    {
    }

    ~Server() { stop(); }

    auto listen(std::uint16_t port, std::string_view host = "0.0.0.0") -> bool
    {
        if (port == 0)
            return false;

        auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host), port);

        return listen(endpoint);
    }

    auto listen(boost::asio::ip::tcp::endpoint const &endpoint) -> bool
    {
        boost::beast::error_code ec;

        // Open the acceptor
        acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return false;
        }

        // Allow address reuse
        acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return false;
        }

        // Bind to the server address
        acceptor.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return false;
        }

        // Start listening for connections
        acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
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

    void doAccept()
    {
        acceptor.async_accept(ioPool.getIOContext(), boost::beast::bind_front_handler(&Server::onAccept, this));
    }

    // Start accepting incoming connections
    void onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
    {
        if (ec) {
            fail(ec, "accept");
        } else {
            // Create the session and run it
            std::make_shared<Connection>(std::move(socket))->run();
        }
        doAccept();
    }

    void stop() { ioPool.stop(); }

private:
    void fail(boost::system::error_code ec, char const *what)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        LogHelper::instance().error("{}:{}", what, ec.message());
    }

    IOPool ioPool{};
    boost::asio::ip::tcp::acceptor acceptor;
};
}  // namespace Uranus::WebSocket
