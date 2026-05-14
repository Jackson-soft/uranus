#pragma once

#include <atomic>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/execution/outstanding_work.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/require.hpp>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

namespace uranus::net {
class IoPool {
public:
    explicit IoPool(const std::uint32_t size = std::thread::hardware_concurrency()) {
        ioContexts_.reserve(size);
        works_.reserve(size);
        for (std::uint32_t i = 0; i < size; i++) {
            auto ioContext = std::make_shared<boost::asio::io_context>();
            works_.emplace_back(
                boost::asio::require(ioContext->get_executor(), boost::asio::execution::outstanding_work_t::tracked));
            ioContexts_.emplace_back(std::move(ioContext));
        }
    }

    ~IoPool() {
        Stop();
    }

    IoPool(const IoPool &)                     = delete;
    auto operator=(const IoPool &) -> IoPool & = delete;
    IoPool(IoPool &&)                          = delete;
    auto operator=(IoPool &&) -> IoPool &      = delete;

    void Run() {
        threads_.reserve(ioContexts_.size());
        for (const auto &ctx : ioContexts_) {
            threads_.emplace_back([&ctx]() {
                ctx->run();
            });
        }

        for (auto &t : threads_) {
            t.join();
        }
        threads_.clear();
    }

    void Stop() {
        works_.clear();
        for (const auto &ctx : ioContexts_) {
            ctx->stop();
        }
    }

    auto IoContext() -> boost::asio::io_context & {
        auto index = next_.fetch_add(1, std::memory_order_relaxed) % ioContexts_.size();
        return *ioContexts_[index];
    }

private:
    std::vector<std::shared_ptr<boost::asio::io_context>> ioContexts_;
    std::vector<boost::asio::any_io_executor>             works_;
    std::vector<std::thread>                              threads_;
    std::atomic<std::uint32_t>                            next_{0};
};
}  // namespace uranus::net
