#pragma once

#include <boost/asio.hpp>
#include <thread>

#include "io_pool.hpp"

namespace Uranus::Net
{
class Server
{
public:
    Server(std::size_t size = std::thread::hardware_concurrency()): ioPool(size) {}

    ~Server() { ioPool.stop(); }

    auto listen(std::uint16_t port, std::string_view host = "0.0.0.0") -> bool
    {
        if (port <= 0)
            return false;

        auto address = boost::asio::ip::make_address(host);
        return true;
    }

    void run()
    {
        doAccept();
        ioPool.run();
    }

private:
    void doAccept()
    {
        acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
            }
            doAccept();
        });
    }

    IOPool ioPool;
    boost::asio::ip::tcp::acceptor acceptor;
};
}  // namespace Uranus::Net