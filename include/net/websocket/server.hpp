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
#include "net/ioPool.hpp"
#include "utils/log.hpp"

namespace uranus::net::websocket
{
using handler = std::function<void()>;

class server
{
public:
    explicit server(std::size_t size = std::thread::hardware_concurrency())
        : iocPool(size), acceptor(iocPool.getIOContext())
    {
    }

    ~server() { stop(); }

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
        iocPool.run();
    }

    void doAccept()
    {
        acceptor.async_accept(iocPool.getIOContext(), boost::beast::bind_front_handler(&server::onAccept, this));
    }

    // Start accepting incoming connections
    void onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
    {
        if (ec) {
            fail(ec, "accept");
        } else {
            // Create the session and run it
            std::make_shared<connection>(std::move(socket))->run();
        }
        doAccept();
    }

    // 设置消息处理回调
    void setHandler() {}

    void stop() { iocPool.stop(); }

private:
    void fail(boost::system::error_code ec, char const *what)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        utils::logHelper::instance().error("{}:{}", what, ec.message());
    }

    ioPool iocPool;
    boost::asio::ip::tcp::acceptor acceptor;
};
}  // namespace uranus::net::websocket
