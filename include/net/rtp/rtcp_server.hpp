#pragma once

#include <cstdint>

namespace uranus::rtp {
// From RFC 3550, RTCP: A Transport Protocol for Real-Time Applications.
//
// RTCP header format.
//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |V=2|P| RC/FMT  |     PT=SR=200       |         length          |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RtcpServer {
public:
    RtcpServer()  = default;
    ~RtcpServer() = default;

private:
    std::uint16_t version_     : 2;  // 2bit 版本号
    std::uint16_t padding_     : 1;  // 1bit 若设置填充比特，该 RTCP 包在末端包含一些附加填充比特，并不是控制信息的基本部分
    std::uint32_t rc_          : 5;  // 5bit 该包中所含接收报告块的数目。零值有效。

    std::uint32_t payloadType_ : 8;  // 8bit 包类型，包含常数 200，用以识别这个为 SR 包。
    std::uint16_t length;  // 16bit 该 RTCP 包的长度减 1。其单位是 32 比特字，包括头和任何填充字节。

    std::uint32_t ssrc_;  // 32bit SR 包发送者的同步源标识符。
};
}  // namespace uranus::rtp
