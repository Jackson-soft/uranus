#pragma once

#include <cstddef>
#include <vector>

namespace uranus::utils
{
template<typename T>
inline auto readByte(const std::vector<std::byte> &data) -> T
{
    T tmp{0};
    if (data.empty())
        return tmp;
    auto size = sizeof(T);
    for (auto i = 0; i < size; ++i) {
        tmp |= std::to_integer<T>(data.at(i)) << ((size - 1 - i) * 8);
    }
    return tmp;
}

template<typename T>
inline auto writeByte(const T &data) -> std::vector<std::byte>
{
    std::vector<std::byte> tmp{};
    auto size = sizeof(T);
    for (unsigned int i = 0; i < size; ++i) {
        tmp.emplace_back(data >> ((size - 1 - i) * 8));
    }

    return tmp;
}

}  // namespace uranus::utils
