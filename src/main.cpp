/*
#include "net/rtp/server.hpp"
#include "net/tcp/server.hpp"
#include "net/websocket/server.hpp"
*/

#include "main.hpp"
#include "utils/log.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <map>
#include <print>
#include <string>
#include <vector>

auto main() -> int {
    /*
    uranus::tcp::Server svr;
    const auto          port{9080};
    svr.Listen(port);
    svr.Run();
    */

    uranus::utils::LogHelper::Instance().Initalize("logs/rotating.log");

    uranus::utils::LogHelper::Instance().Info("hello world");
    uranus::utils::LogHelper::Instance().Info("hello world {}", "info");
    uranus::utils::LogHelper::Instance().Error("hello world {}", "error");

    std::vector<int>   vInt{2, 3, 45, 65};
    std::array<int, 4> aInt{0, 9, 8, 8};

    std::print("before vInt {}, aInt {}\n", vInt, aInt);

    std::copy_n(vInt.begin(), 4, aInt.begin());
    std::print("copy_n vInt {}, aInt {}\n", vInt, aInt);

    std::move(vInt.begin(), vInt.begin() + 4, aInt.begin());
    std::print("move vInt {}, aInt {}\n", vInt, aInt);
    std::print("ffff {}\n", "soooooooo");

    std::map<std::string, std::string> vMap{{"1", "1->1"}};

    Write(1, 5.0, "fsf", vInt, vMap);

    return EXIT_SUCCESS;
}
