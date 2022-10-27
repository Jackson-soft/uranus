/*
#include "net/rtp/server.hpp"
#include "net/tcp/server.hpp"
#include "net/websocket/server.hpp"
*/
#include "fmt/core.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <fmt/ranges.h>
#include <vector>

auto main() -> int {
    /*
    uranus::tcp::Server svr;
    const auto          port{9080};
    svr.Listen(port);
    svr.Run();
    */
    fmt::print(stdout, "ffff\n");

    std::vector<int>   vInt{2, 3, 45, 65};
    std::array<int, 4> aInt{0, 9, 8, 8};

    fmt::print(stdout, "before vInt {}, aInt {}\n", vInt, aInt);

    std::copy_n(vInt.begin(), 4, aInt.begin());
    fmt::print(stdout, "copy_n vInt {}, aInt {}\n", vInt, aInt);

    std::move(vInt.begin(), vInt.begin() + 4, aInt.begin());
    fmt::print(stdout, "move vInt {}, aInt {}\n", vInt, aInt);

    return EXIT_SUCCESS;
}
