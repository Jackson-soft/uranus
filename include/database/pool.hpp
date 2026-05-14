#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>

namespace uranus::database {

template<typename Conn>
class Pool {
public:
    using ConnPtr   = std::shared_ptr<Conn>;
    using Factory   = std::function<ConnPtr(const std::string &)>;
    using Validator = std::function<bool(const ConnPtr &)>;

    struct Config {
        std::string dsn;
        std::size_t min_size{2};
        std::size_t max_size{10};
    };

    static auto Instance() -> Pool & {
        static Pool instance;
        return instance;
    }

    auto Initialize(Config config, Factory factory, Validator validator = nullptr) -> bool {
        std::call_once(flag_, [this, &config, &factory, &validator]() {
            config_    = std::move(config);
            factory_   = std::move(factory);
            validator_ = std::move(validator);

            for (std::size_t i = 0; i < config_.min_size; ++i) {
                if (auto conn = createConnection(); conn) {
                    idle_.push(std::move(conn));
                    ++total_;
                }
            }

            ready_.store(total_ > 0);
        });
        return ready_.load();
    }

    auto Acquire(std::chrono::milliseconds timeout = std::chrono::milliseconds{3000}) -> ConnPtr {
        std::unique_lock lock(mutex_);

        while (idle_.empty()) {
            if (total_ < config_.max_size) {
                ++total_;
                lock.unlock();
                auto conn = createConnection();
                if (!conn) {
                    lock.lock();
                    --total_;
                    return nullptr;
                }
                return conn;
            }

            if (cv_.wait_for(lock, timeout) == std::cv_status::timeout) {
                return nullptr;
            }
        }

        auto conn = std::move(idle_.front());
        idle_.pop();

        if (validator_ && !validator_(conn)) {
            --total_;
            lock.unlock();
            auto fresh = createConnection();
            if (fresh) {
                std::lock_guard relock(mutex_);
                ++total_;
            }
            return fresh;
        }

        return conn;
    }

    void Release(ConnPtr conn) {
        if (!conn)
            return;

        std::lock_guard lock(mutex_);
        if (idle_.size() < config_.max_size) {
            idle_.push(std::move(conn));
            cv_.notify_one();
        } else {
            --total_;
        }
    }

    class Guard {
    public:
        Guard(Pool &pool, ConnPtr conn) : pool_(pool), conn_(std::move(conn)) {}

        ~Guard() {
            if (conn_)
                pool_.Release(std::move(conn_));
        }

        Guard(const Guard &)                     = delete;
        auto operator=(const Guard &) -> Guard & = delete;

        Guard(Guard &&other) noexcept : pool_(other.pool_), conn_(std::move(other.conn_)) {}

        auto operator=(Guard &&) -> Guard & = delete;

        auto operator->() const -> Conn * {
            return conn_.get();
        }

        auto operator*() const -> Conn & {
            return *conn_;
        }

        explicit operator bool() const {
            return conn_ != nullptr;
        }

        auto Get() const -> const ConnPtr & {
            return conn_;
        }

    private:
        Pool   &pool_;
        ConnPtr conn_;
    };

    auto AcquireGuard(std::chrono::milliseconds timeout = std::chrono::milliseconds{3000}) -> Guard {
        return Guard(*this, Acquire(timeout));
    }

    [[nodiscard]] auto Size() -> std::size_t {
        std::lock_guard lock(mutex_);
        return idle_.size();
    }

    [[nodiscard]] auto Total() const -> std::size_t {
        return total_.load();
    }

    [[nodiscard]] auto Ready() const -> bool {
        return ready_.load();
    }

private:
    Pool() = default;

    auto createConnection() -> ConnPtr {
        try {
            return factory_(config_.dsn);
        } catch (...) {
            return nullptr;
        }
    }

    Config    config_;
    Factory   factory_;
    Validator validator_;

    std::queue<ConnPtr>      idle_;
    std::mutex               mutex_;
    std::condition_variable  cv_;
    std::once_flag           flag_;
    std::atomic_bool         ready_{false};
    std::atomic<std::size_t> total_{0};
};

}  // namespace uranus::database
