#include "net/rtp/server.hpp"
#include "net/tcp/server.hpp"
#include "net/websocket/server.hpp"

#include <cstdlib>

auto main() -> int
{
    uranus::tcp::Server svr;
    const auto          port{9080};
    svr.listen(port);
    svr.run();

    return EXIT_SUCCESS;
}
