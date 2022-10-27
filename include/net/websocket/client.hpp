#pragma once

#include "utils/log.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
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

namespace uranus::websocket {
class Client {
public:
    explicit Client() : resolver_(ioContext), ws_(ioContext) {}

    ~Client() {
        resolver_.cancel();
        Close();
    }

    // uri like ws://localhost:9002/xxx
    auto Connect(std::string_view uri) -> bool {
        if (uri.empty()) {
            return false;
        }
        return false;
    }

    auto Connect(std::string_view host, std::string_view port, std::string_view path = "/") -> bool {
        if (host.empty() || port.empty()) {
            return false;
        }

        // Look up the domain name
        auto const results = resolver_.resolve(host, port);

        // Set a timeout on the operation
        boost::beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        auto ep = boost::beast::get_lowest_layer(ws_).connect(results);

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        boost::beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type &req) {
            req.set(boost::beast::http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
        }));

        std::string address{host};
        address += ':' + std::to_string(ep.port());

        // Perform the websocket handshake
        ws_.handshake(address, path.data());

        return true;
    }

    void Run() {
        ioContext.run();
    }

    void Write(std::string_view text) {
        if (text.empty()) {
            return;
        }
        boost::asio::post(ioContext, [this, text] {
            OnWrite(text);
        });
    }

    void OnWrite(std::string_view msg) {
        responses_.emplace(msg);
        ws_.async_write(boost::asio::buffer(responses_.front()),
                        boost::beast::bind_front_handler(&Client::doWrite, this));
    }

    void DoWrite(boost::system::error_code ec, std::size_t) {
        if (ec) {
            return fail(ec, "write");
        }

        responses_.pop();

        if (!responses_.empty()) {
            ws_.async_write(boost::asio::buffer(responses_.front()),
                            boost::beast::bind_front_handler(&Client::doWrite, this));
        }
    }

    void Read() {
        // Read a message into our buffer
        ws_.async_read(buffer_, boost::beast::bind_front_handler(&Client::OnRead, this));
    }

    void OnRead(boost::system::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            return fail(ec, "read");
        }

        buffer_.consume(bytes_transferred);
    }

    void Close() {
        ws_.close(boost::beast::websocket::close_code::normal);
    }

private:
    void fail(boost::system::error_code ec, std::string_view what) {
        uranus::utils::LogHelper::Instance().Error("{}:{}", what, ec.message());
    }

    boost::asio::io_context                                   ioContext;
    boost::asio::ip::tcp::resolver                            resolver_;
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::multi_buffer                                buffer_;
    std::queue<std::string>                                   responses_;
};
}  // namespace uranus::websocket
