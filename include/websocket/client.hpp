#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <string_view>

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
        if (ec) {
            fail(ec, "resolve");
            return false;
        }

        // Set a timeout on the operation
        boost::beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        auto ep = boost::beast::get_lowest_layer(ws).connect(results);
        if (ec) {
            fail(ec, "connect");
            return false;
        }

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
        if (ec) {
            fail(ec, "handshake");
            return false;
        }

        return true;
    }

    void run()
    {
        // boost::asio::spawn(ioContext,std::bind());
        ioContext.run();
    }

    void write(std::string_view text)
    {
        // Send the message
        /*
        ws.async_write(boost::asio::buffer(std::string(text)), [ec]);
        if (ec)
            return fail(ec, "write");
            */
    }

    void read()
    {
        // This buffer will hold the incoming message

        boost::beast::flat_buffer buffer;

        // Read a message into our buffer
        /*
        ws.async_read(buffer, [ec]);
        if (ec)
            return fail(ec, "read");
            */
    }

    auto close() -> bool
    {
        ws.close(boost::beast::websocket::close_code::normal);
        return true;
    }

    void fail(boost::beast::error_code ec, std::string_view what) { std::cerr << what << ": " << ec.message() << "\n"; }

private:
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::resolver resolver;
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
    boost::beast::error_code ec;
};
}  // namespace Uranus::WebSocket
