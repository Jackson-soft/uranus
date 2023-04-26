#pragma once

#include <algorithm>
#include <any>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

/*
https://redis.io/docs/reference/protocol-spec
https://github.com/antirez/RESP3/blob/master/spec.md
*/

namespace uranus::redis {

// redis resp protocol data type.
const char RespStatus{'+'};     // +<string>\r\n
const char RespError{'-'};      // -<string>\r\n
const char RespString{'$'};     // $<length>\r\n<bytes>\r\n
const char RespInt{':'};        // :<number>\r\n
const char RespNil{'_'};        // _\r\n
const char RespFloat{','};      // ,<floating-point-number>\r\n (golang float)
const char RespBool{'#'};       // true: #t\r\n false: #f\r\n
const char RespBlobError{'!'};  // !<length>\r\n<bytes>\r\n
const char RespVerbatim{'='};   // =<length>\r\nFORMAT:<bytes>\r\n
const char RespBigInt{'('};     // (<big number>\r\n
const char RespArray{'*'};      // *<len>\r\n... (same as resp2)
const char RespMap{'%'};        // %<len>\r\n(key)\r\n(value)\r\n... (golang map)
const char RespSet{'~'};        // ~<len>\r\n... (same as Array)
const char RespAttr{'|'};       // |<len>\r\n(key)\r\n(value)\r\n... + command reply
const char RespPush{'>'};       // ><len>\r\n... (same as Array)

constexpr char const *separator = "\r\n";

class Packet {
public:
    Packet()  = default;
    ~Packet() = default;

    /*
    *<number of arguments> CR LF
    $<number of bytes of argument 1> CR LF
    <argument data> CR LF
    ...
    $<number of bytes of argument N> CR LF
    <argument data> CR LF

    Clients send commands to a Redis server as a RESP Array of Bulk Strings.
    example: set mykey myvalue => *3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n
    */
    auto Write(const std::vector<std::string> &data) -> std::vector<char> {
        buffer_.clear();
        buffer_.emplace_back(RespArray);

        header(data.size());

        std::for_each(data.begin(), data.end(), [this](const auto &arg) {
            WriteArg(arg);
        });

        return buffer_;
    }

    // 写入单条命令
    void WriteArg(const std::string &arg) {
        auto              buf = std::any_cast<std::string>(arg);
        std::vector<char> data(buf.begin(), buf.end());
        bytes(data);
    }

    void Reset() {
        buffer_.clear();
    }

private:
    void header(std::size_t len) {
        auto lens = std::to_string(len);
        std::for_each(lens.begin(), lens.end(), [this](auto const &arg) {
            buffer_.emplace_back(arg);
        });
        crlf();
    }

    void bytes(const std::vector<char> &data) {
        buffer_.emplace_back(RespString);
        header(data.size());
        std::for_each(data.begin(), data.end(), [this](const auto &arg) {
            buffer_.emplace_back(arg);
        });
        crlf();
    }

    void crlf() {
        buffer_.emplace_back('\r');
        buffer_.emplace_back('\n');
    }

    std::vector<char> buffer_;
};
}  // namespace uranus::redis
