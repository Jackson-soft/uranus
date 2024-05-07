#pragma once

#include "utils/log.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

//
namespace uranus::websocket {
class Connection : public std::enable_shared_from_this<Connection> {
public:
    explicit Connection(boost::asio::ip::tcp::socket &&socket) : ws_(std::move(socket)), timer_(socket.get_executor()) {
        timer_.expires_at(std::chrono::steady_clock::time_point::max());
    }

    ~Connection() {
        close();
    }

    void run() {
        // Set suggested timeout settings for the websocket
        ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type &res) {
            res.set(boost::beast::http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
        }));

        // Accept the websocket handshake
        boost::asio::co_spawn(
            ws_.get_executor(),
            [self = shared_from_this()] {
                return self->acceptor();
            },
            boost::asio::detached);
    }

    // 远程地址
    void remote() {}

    // 本地地址
    void local() {}

    void close() {
        ws_.close(boost::beast::websocket::close_code::normal);
    }

private:
    // Report a failure
    void fail(boost::system::error_code ec, std::string_view what) {
        if (ec == boost::asio::error::operation_aborted || ec == boost::beast::websocket::error::closed) {
            return;
        }

        uranus::utils::LogHelper::Instance().Error("{}:{}", what, ec.message());
    }

    auto acceptor() -> boost::asio::awaitable<void> {
        co_await ws_.async_accept(boost::asio::use_awaitable);

        while (true) {
            auto bytes = co_await ws_.async_read(buffer_, boost::asio::use_awaitable);
            if (bytes > 0) {
                auto message = boost::beast::buffers_to_string(buffer_.data());

                uranus::utils::LogHelper::Instance().Info("read bytes: {}, message: {}", bytes, message);

                // onMessage(message.data());

                buffer_.consume(message.size());
            }

            if (!responses_.empty()) {
                ws_.text(ws_.got_text());
                co_await ws_.async_write(boost::asio::buffer(responses_.front()), boost::asio::use_awaitable);
                responses_.pop();
            }
        }
    }

    void callback() {}

    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::asio::steady_timer                                 timer_;
    boost::beast::flat_buffer                                 buffer_;
    std::queue<std::string>                                   responses_;
    std::mutex                                                mtx_;
};
}  // namespace uranus::websocket
