#pragma once

#include <type_traits>
#include <vector>

namespace uranus::utils {
template<typename T>
requires std::is_arithmetic_v<T>
inline auto ReadByte(std::vector<char> &data) -> T {
    T tmp{0};
    if (data.empty()) {
        return tmp;
    }

    const auto size = sizeof(T);
    for (auto i = 0; i < size; ++i) {
        tmp |= data.at(i) << ((size - 1 - i) * 8);
    }
    return tmp;
}

template<typename T>
requires std::is_arithmetic_v<T>
inline auto WriteByte(const T &data) -> std::vector<char> {
    std::vector<char> tmp{};
    const auto              size = sizeof(T);

    for (auto i = 0; i < size; ++i) {
        tmp.emplace_back(data >> ((size - 1 - i) * 8));
    }

    return tmp;
}

}  // namespace uranus::utils
