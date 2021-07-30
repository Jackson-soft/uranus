#pragma once

#include "connection.hpp"
#include "net/base/io_pool.hpp"
#include "utils/log.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace uranus::websocket {
class server {
public:
    explicit server(std::uint32_t size = std::thread::hardware_concurrency())
        : iocPool_(size), acceptor_(iocPool_.getIoContext())
    {
    }

    ~server()
    {
        stop();
    }

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
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return false;
        }

        // Allow address reuse
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return false;
        }

        acceptor_.non_blocking(true);

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return false;
        }

        // Start listening for connections
        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return false;
        }

        return true;
    }

    void run()
    {
        boost::asio::co_spawn(acceptor_.get_executor(), onAccept(), boost::asio::detached);
        iocPool_.run();
    }

    // 设置消息处理回调
    void setHandler() {}

    void stop()
    {
        iocPool_.stop();
    }

private:
    void fail(boost::system::error_code ec, char const *what)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        // utils::logHelper::instance().error("{}:{}", what, ec.message());
    }

    auto onAccept() -> boost::asio::awaitable<void>
    {
        while (true) {
            auto socket = co_await acceptor_.async_accept(boost::asio::use_awaitable);
            // auto ep     = socket.remote_endpoint();
            auto conn   = std::make_shared<uranus::websocket::Connection>(std::move(socket));
            conn->run();
        }
    }

    uranus::net::IoPool            iocPool_{};
    boost::asio::ip::tcp::acceptor acceptor_;
};
}  // namespace uranus::websocket
