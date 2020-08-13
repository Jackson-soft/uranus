#pragma once

#include <boost/asio/buffer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/make_printable.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

#include "log.hpp"

//
namespace Uranus::WebSocket
{
class Connection: public std::enable_shared_from_this<Connection>
{
public:
    explicit Connection(boost::asio::ip::tcp::socket &&socket): ws(std::move(socket)) {}

    ~Connection() = default;

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
                    std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
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

        if (ec) {
            fail(ec, "read");
        }else {
            std::cout << boost::beast::make_printable(buffer.data()) << std::endl;

            // consume the data
            buffer.consume(buffer.size());

            // Read another message
            ws.async_read(buffer, boost::beast::bind_front_handler(&Connection::onRead, shared_from_this()));
        }
    }

    void write(std::string_view text)
    {
        std::lock_guard<std::mutex> lock(mtx);
        writeMsgs.emplace(text);
        ws.text(ws.got_text());
        ws.async_write(boost::asio::buffer(writeMsgs.front()),
                       boost::beast::bind_front_handler(&Connection::doWrite, shared_from_this()));
    }

    void doWrite(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");
        writeMsgs.pop();

        if (!writeMsgs.empty()) {
            ws.text(ws.got_text());
            ws.async_write(boost::asio::buffer(writeMsgs.front()),
                           boost::beast::bind_front_handler(&Connection::doWrite, shared_from_this()));
        }
    }

    void close() { ws.close(boost::beast::websocket::close_code::normal); }

private:
    // Report a failure
    void fail(boost::system::error_code ec, std::string_view what)
    {
        if (ec == boost::asio::error::operation_aborted || ec == boost::beast::websocket::error::closed)
            return;

        LogHelper::instance().error("{}:{}", what, ec.message());
    }

    void callback() {}

    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
    boost::beast::flat_buffer buffer;
    std::queue<std::string> writeMsgs;
    std::mutex mtx;
};
}  // namespace Uranus::WebSocket
