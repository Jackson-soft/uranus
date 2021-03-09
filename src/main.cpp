#include "net/rtp/server.hpp"
#include "net/tcp/server.hpp"
#include "net/websocket/server.hpp"
#include <cstdlib>

#include "utils/log.hpp"

auto main() -> int
{
    uranus::tcp::Server svr;
    svr.listen(9080);
    svr.run();

    return EXIT_SUCCESS;
}
