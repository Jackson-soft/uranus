#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

#include "utils/log.hpp"

//
namespace uranus::tcp
{
class connection: public std::enable_shared_from_this<connection>
{
public:
    explicit connection(boost::asio::ip::tcp::socket &&socket)
        : socket_(std::move(socket)), timer_(socket_.get_executor())
    {
        timer_.expires_at(std::chrono::steady_clock::time_point::max());
    }

    ~connection() = default;

    void run()
    {
        boost::asio::co_spawn(
            socket_.get_executor(), [self = shared_from_this()] { return self->reader(); }, boost::asio::detached);

        boost::asio::co_spawn(
            socket_.get_executor(), [self = shared_from_this()] { return self->writer(); }, boost::asio::detached);
    }

    // 远程地址
    void remote() {}

    // 本地地址
    void local() {}

    void stop()
    {
        socket_.close();
        timer_.cancel();
    }

private:
    // Report a failure
    void fail(boost::system::error_code ec, std::string_view what)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        // utils::logHelper::instance().error("{}:{}", what, ec.message());
    }

    auto reader() -> boost::asio::awaitable<void>
    {
        try {
            for (std::string responses_;;) {
                auto bytes = co_await boost::asio::async_read_until(
                    socket_, boost::asio::dynamic_buffer(responses_, 1024), "\n", boost::asio::use_awaitable);
            }
        } catch (std::exception &) {
            stop();
        }
    }

    auto writer() -> boost::asio::awaitable<void>
    {
        try {
            while (socket_.is_open()) {
                if (responses_.empty()) {
                    boost::system::error_code ec;
                    co_await timer_.async_wait(redirect_error(boost::asio::use_awaitable, ec));
                } else {
                    co_await boost::asio::async_write(
                        socket_, boost::asio::buffer(responses_.front()), boost::asio::use_awaitable);
                    responses_.pop();
                }
            }
        } catch (std::exception &) {
            stop();
        }
    }

    boost::asio::ip::tcp::socket socket_;
    boost::asio::steady_timer timer_;
    std::queue<std::string> responses_;
    std::mutex mtx_;
};
}  // namespace uranus::tcp
