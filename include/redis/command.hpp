#pragma once

#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <vector>

namespace uranus::redis {
// redis 命令类
template<typename T>
class Cmder {
public:
    Cmder()  = default;
    ~Cmder() = default;

    template<typename... Args>
    void Push(std::string_view cmd, Args const &...args) {
        auto constexpr packSize = sizeof...(Args);
        args_.reserve(packSize + 1);
        args_.emplace_back(cmd);
        add(std::tie(std::forward<Args const &>(args)...));
    }

    auto Args() -> std::vector<std::string> {
        return args_;
    }

    auto Result() -> T {
        return value_;
    }

private:
    template<typename... Args>
    void add(std::tuple<Args...> const &args) {
        auto fun = [&](auto const &...arg) {
            (add(arg), ...);
        };

        std::apply(fun, args);
    }

    template<typename Arg>
    requires std::is_integral_v<Arg>
    void add(Arg arg) {
        auto const str = std::to_string(arg);
        add(std::string_view{str});
    }

    void add(std::string_view arg) {
        args_.push_back(arg);
    }

    std::vector<std::string> args_;
    std::error_code          error_;
    T                        value_;
};
}  // namespace uranus::redis
