#pragma once

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace uranus::utils {
template<typename T>
requires std::is_arithmetic_v<T>
auto ReadByte(const std::vector<char> &data) -> T {
    T tmp{0};
    if (data.empty()) {
        return tmp;
    }

    constexpr auto size = sizeof(T);
    const auto     len  = std::min(size, data.size());
    for (std::size_t i = 0; i < len; ++i) {
        tmp |= static_cast<T>(static_cast<unsigned char>(data[i])) << ((size - 1 - i) * 8);
    }
    return tmp;
}

template<typename T>
requires std::is_arithmetic_v<T>
auto WriteByte(T data) -> std::vector<char> {
    std::vector<char> tmp;
    constexpr auto    size = sizeof(T);
    tmp.reserve(size);

    for (std::size_t i = 0; i < size; ++i) {
        tmp.emplace_back(static_cast<char>((data >> ((size - 1 - i) * 8)) & 0xFF));
    }

    return tmp;
}

}  // namespace uranus::utils
