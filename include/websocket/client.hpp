#pragma once

#include <bits/c++config.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <string_view>

#include "log.hpp"

namespace Uranus::WebSocket
{
class Client
{
public:
    explicit Client(): resolver(ioContext), ws(ioContext) {}

    ~Client()
    {
        resolver.cancel();
        close();
    }

    // uri like ws://localhost:9002/xxx
    auto connect(std::string_view uri) -> bool
    {
        if (uri.empty())
            return false;
        return false;
    }

    auto connect(std::string_view host, std::string_view port, std::string_view path = "/") -> bool
    {
        if (host.empty() || port.empty())
            return false;

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Set a timeout on the operation
        boost::beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        auto ep = boost::beast::get_lowest_layer(ws).connect(results);

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        boost::beast::get_lowest_layer(ws).expires_never();

        // Set suggested timeout settings for the websocket
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type &req) {
            req.set(boost::beast::http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
        }));

        std::string address{host};
        address += ':' + std::to_string(ep.port());

        // Perform the websocket handshake
        ws.handshake(address, path.data());

        return true;
    }

    void run()
    {
        // boost::asio::spawn(ioContext,std::bind());
        ioContext.run();
    }

    void write(std::string_view text)
    {
        if (text.empty())
            return;
        boost::asio::post(ioContext, [this, text] { onWrite(text); });
    }

    void onWrite(std::string_view msg)
    {
        writeMsgs.emplace(msg);
        ws.async_write(boost::asio::buffer(writeMsgs.front()),
                       boost::beast::bind_front_handler(&Client::doWrite, this));
    }

    void doWrite(boost::system::error_code ec, std::size_t)
    {
        if (ec)
            return fail(ec, "write");

        writeMsgs.pop();

        if (!writeMsgs.empty()) {
            ws.async_write(boost::asio::buffer(writeMsgs.front()),
                           boost::beast::bind_front_handler(&Client::doWrite, this));
        }
    }

    void read()
    {
        // Read a message into our buffer
        ws.async_read(buffer, boost::beast::bind_front_handler(&Client::onRead, this));
    }

    void onRead(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        buffer.consume(bytes_transferred);
    }

    void close() { ws.close(boost::beast::websocket::close_code::normal); }

private:
    void fail(boost::system::error_code ec, std::string_view what)
    {
        LogHelper::instance().error("{}:{}", what, ec.message());
    }

    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;

    boost::beast::multi_buffer buffer;
    std::queue<std::string> writeMsgs;
};
}  // namespace Uranus::WebSocket
