#pragma once

#include <algorithm>
#include <any>
#include <charconv>
#include <cstddef>
#include <string>
#include <vector>

/*
https://redis.io/docs/reference/protocol-spec
https://github.com/antirez/RESP3/blob/master/spec.md
*/

namespace uranus::redis {

// redis resp protocol data type.
const std::byte RespError{'-'};      // -<string>\r\n
const std::byte RespStatus{'+'};     // +<string>\r\n
const std::byte RespString{'$'};     // $<length>\r\n<bytes>\r\n
const std::byte RespInt{':'};        // :<number>\r\n
const std::byte RespNil{'_'};        // _\r\n
const std::byte RespFloat{','};      // ,<floating-point-number>\r\n (golang float)
const std::byte RespBool{'#'};       // true: #t\r\n false: #f\r\n
const std::byte RespBlobError{'!'};  // !<length>\r\n<bytes>\r\n
const std::byte RespVerbatim{'='};   // =<length>\r\nFORMAT:<bytes>\r\n
const std::byte RespBigInt{'('};     // (<big number>\r\n
const std::byte RespArray{'*'};      // *<len>\r\n... (same as resp2)
const std::byte RespMap{'%'};        // %<len>\r\n(key)\r\n(value)\r\n... (golang map)
const std::byte RespSet{'~'};        // ~<len>\r\n... (same as Array)
const std::byte RespAttr{'|'};       // |<len>\r\n(key)\r\n(value)\r\n... + command reply
const std::byte RespPush{'>'};       // ><len>\r\n... (same as Array)

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
    template<typename... Args>
    auto Write(Args... args) -> std::vector<std::byte> {
        std::vector<std::any> data{args...};

        buffer_.clear();
        buffer_.emplace_back(RespArray);

        writeLen(data.size());

        std::for_each(data.begin(), data.end(), [this](const auto &arg) {
            WriteArg(arg);
        });

        return buffer_;
    }

    // 写入单条命令
    void WriteArg(std::any arg) {
        if (arg.type() == typeid(std::string)) {
        } else if (arg.type() == typeid(int)) {
            // buffer_.emplace_back(static_cast<std::byte>(std::any_cast<int>(arg)));
        } else if (arg.type() == typeid(double)) {
        }
    }

    auto Read() -> int {
        return 0;
    }

private:
    void writeLen(std::size_t len) {
        buffer_.emplace_back(static_cast<std::byte>(len));
        crlf();
    }

    void bytes(const std::vector<std::byte> &data) {
        buffer_.emplace_back(RespString);
        crlf();
    }

    void crlf() {
        buffer_.emplace_back(std::byte('\r'));
        buffer_.emplace_back(std::byte('\n'));
    }

    std::vector<std::byte> buffer_;
};
}  // namespace uranus::redis
