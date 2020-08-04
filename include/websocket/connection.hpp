#pragma once

#include <boost/asio/dispatch.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <memory>
#include <queue>
#include <string_view>
#include <utility>

//
namespace Uranus::WebSocket
{
class Connection: std::enable_shared_from_this<Connection>
{
public:
    explicit Connection(boost::asio::ip::tcp::socket &&socket): ws(std::move(socket)) {}
    ~Connection() { close(); }

    void run()
    {
        boost::asio::dispatch(ws.get_executor(),
                              boost::beast::bind_front_handler(&Connection::onRun, shared_from_this()));
    }

    void onRun()
    {

        // Set suggested timeout settings for the websocket
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
            res.set(boost::beast::http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-stackless");
        }));

        // Accept the websocket handshake
        ws.async_accept(boost::beast::bind_front_handler(&Connection::onAccept, shared_from_this()));
    }

    void onAccept(boost::system::error_code ec)
    {
        if (ec)
            return fail(ec, "accept");

        // Read a message into our buffer
        ws.async_read(buffer, boost::beast::bind_front_handler(&Connection::onRead, shared_from_this()));
    }

    void onRead(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the session was closed
        if (ec == boost::beast::websocket::error::closed)
            return;

        if (ec)
            fail(ec, "read");

        // Echo the message
        ws.text(ws.got_text());
        // ws.async_write(buffer.data(), boost::beast::bind_front_handler(&Connection::onWrite, shared_from_this()));
    }

    void onWrite(std::string_view text) {}

    void close() { ws.close(boost::beast::websocket::close_code::normal); }

private:
    // Report a failure
    void fail(boost::system::error_code ec, std::string_view what)
    {
        if (ec == boost::asio::error::operation_aborted || ec == boost::beast::websocket::error::closed)
            return;
        std::cerr << what << ": " << ec.message() << "\n";
    }

    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
    boost::beast::flat_buffer buffer;
};  // namespace Aquarius::WebSocket
}  // namespace Aquarius::WebSocket
