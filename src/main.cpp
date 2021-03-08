#include "net/tcp/server.hpp"
#include "net/websocket/server.hpp"
#include <cstdlib>

auto main() -> int
{
    uranus::tcp::Server svr;
    svr.listen(9090);
    svr.run();

    return EXIT_SUCCESS;
}
