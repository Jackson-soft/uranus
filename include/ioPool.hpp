#pragma once

#include <bits/c++config.h>
#include <boost/asio/io_context.hpp>
#include <boost/core/noncopyable.hpp>
#include <list>
#include <memory>
#include <thread>
#include <vector>

namespace uranus
{
class ioPool: public boost::noncopyable
{
public:
    explicit ioPool(std::size_t size = std::thread::hardware_concurrency())
    {
        ioContexts.reserve(size);
        for (std::size_t i = 0; i < size; i++) {
            auto ioContext = std::make_shared<boost::asio::io_context>();
            ioContexts.emplace_back(ioContext);
            works.emplace_back(boost::asio::make_work_guard(*ioContext));
        }
    }

    ~ioPool() { stop(); }

    void run()
    {
        std::vector<std::shared_ptr<std::thread>> threads;
        threads.reserve(ioContexts.size());

        for (auto const &it : ioContexts) {
            threads.emplace_back(std::make_shared<std::thread>(
                [](std::shared_ptr<boost::asio::io_context> const &svr) { svr->run(); }, it));
        }

        for (auto &it : threads) {
            it->join();
        }
    }

    void stop()
    {
        works.clear();
        for (auto &it : ioContexts) {
            it->stop();
        }
    }

    auto getIOContext() -> boost::asio::io_context &
    {
        boost::asio::io_context &ioContext = *ioContexts.at(nextIOContext);
        ++nextIOContext;
        if (nextIOContext == ioContexts.size()) {
            nextIOContext = 0;
        }

        return ioContext;
    }

private:
    std::vector<std::shared_ptr<boost::asio::io_context>> ioContexts;
    std::list<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> works;
    std::size_t nextIOContext{0};
};
}  // namespace uranus
