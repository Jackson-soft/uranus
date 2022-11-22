#pragma once

#include <string>
#include <vector>

namespace uranus::redis {

/*
Tcp connection:
    redis://<user>:<password>@<host>:<port>/<db_number>
Unix connection:
    unix://<user>:<password>@</path/to/redis.sock>?db=<db_number>
Examples:
    redis://user:password@localhost:6789/3?dial_timeout=3&db=1&read_timeout=6s&max_retries=2
    redis-cluster://user:password@localhost:67891,localhost:67892/?db=1
*/
class Options {
public:
    Options()  = default;
    ~Options() = default;

private:
    // is or not cluster.
    bool isCluster_{false};
    // The network type, either tcp or unix.
    // Default is tcp.
    std::string network_{"tcp"};

    // A seed list of host:port addresses of nodes.
    std::vector<std::string> address_{{"localhost:6379"}};

    std::string userName_;
    std::string password_;
    int         database_{0};
    int         poolSize_{10};
};
}  // namespace uranus::redis
