#pragma once

#include <stdlib.h>

namespace uranus::utils {
// 守护进程
inline auto daemon() -> bool {
    return ::daemon(1, 1) == 0;
}
}  // namespace uranus::utils
