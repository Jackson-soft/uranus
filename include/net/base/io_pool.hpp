#pragma once

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/execution/outstanding_work.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/require.hpp>
#include <boost/core/noncopyable.hpp>
#include <cstdint>
#include <list>
#include <memory>
#include <thread>
#include <vector>

namespace uranus::net {
class IoPool : public boost::noncopyable {
public:
    explicit IoPool(const std::uint32_t size = std::thread::hardware_concurrency()) {
        ioContexts_.reserve(size);
        for (std::uint32_t i = 0; i < size; i++) {
            auto ioContext = std::make_shared<boost::asio::io_context>();
            ioContexts_.emplace_back(ioContext);
            works_.emplace_back(
                boost::asio::require(ioContext->get_executor(), boost::asio::execution::outstanding_work_t::tracked));
        }
    }

    ~IoPool() {
        Stop();
    }

    void Run() {
        std::vector<std::shared_ptr<std::thread>> threads;
        threads.reserve(ioContexts_.size());

        for (auto const &it : ioContexts_) {
            threads.emplace_back(std::make_shared<std::thread>(
                [](std::shared_ptr<boost::asio::io_context> const &svr) {
                    svr->run();
                },
                it));
        }

        for (auto &it : threads) {
            it->join();
        }
    }

    void Stop() {
        works_.clear();
        for (const auto &it : ioContexts_) {
            it->stop();
        }
    }

    auto IoContext() -> boost::asio::io_context & {
        boost::asio::io_context &ioContext = *ioContexts_.at(next_);
        ++next_;
        if (next_ == ioContexts_.size()) {
            next_ = 0;
        }

        return ioContext;
    }

private:
    std::vector<std::shared_ptr<boost::asio::io_context>> ioContexts_{};
    std::list<boost::asio::any_io_executor>               works_{};
    std::uint32_t                                         next_{0};
};
}  // namespace uranus::net
