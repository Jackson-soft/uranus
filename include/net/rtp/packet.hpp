#pragma once

#include "utils/byte_io.hpp"
#include "utils/log.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include "spdlog/spdlog.h"

namespace uranus::rtp
{
// rtp 解包封包

//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |V=2|P|X|  CC   |M|     PT      |       sequence number         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           timestamp                           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           synchronization source (SSRC) identifier            |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |            Contributing source (CSRC) identifiers             |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |  header eXtension profile id  |       length in 32bits        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          Extensions                           |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |                           Payload                             |
// |             ....              :  padding...                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |               padding         | Padding size  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

constexpr std::size_t kFixedHeaderSize              = 12;
constexpr std::uint8_t kRtpVersion                  = 2;
constexpr std::uint16_t kOneByteExtensionProfileId  = 0xBEDE;
constexpr std::uint16_t kTwoByteExtensionProfileId  = 0x1000;
constexpr std::size_t kOneByteExtensionHeaderLength = 1;
constexpr std::size_t kTwoByteExtensionHeaderLength = 2;
constexpr std::size_t kDefaultPacketSize            = 1500;

class RtpPacket
{
public:
    RtpPacket()  = default;
    ~RtpPacket() = default;

    // 解包
    auto unpack(std::vector<std::byte> &&data) -> bool { return parse(std::move(data)); }

    // 封包
    auto pack() -> bool { return true; }

    [[nodiscard]] auto data() const -> bool { return true; }

private:
    // auto parse(const std::uint8_t *buffer, std::size_t size) -> bool
    auto parse(std::vector<std::byte> &&buffer) -> bool
    {
        if (buffer.size() < kFixedHeaderSize) {
            return false;
        }

        version_ = std::to_integer<std::uint16_t>(buffer.at(0) >> 6);
        if (version_ != kRtpVersion) {
            return false;
        }

        std::byte tmp{0x20};
        padding_ = std::to_integer<bool>(buffer.at(0) & tmp);

        tmp        = std::byte{0x10};
        extension_ = std::to_integer<bool>(buffer.at(0) & tmp);

        tmp   = std::byte{0x0f};
        csrc_ = std::to_integer<std::uint16_t>(buffer.at(0) & tmp);

        tmp     = std::byte{0x80};
        marker_ = std::to_integer<bool>(buffer.at(1) & tmp);

        tmp          = std::byte{0x7f};
        payloadType_ = std::to_integer<std::uint16_t>(buffer.at(1) & tmp);

        std::vector<std::byte> sq;
        std::copy_n(buffer.begin() + 2, 2, std::back_inserter(sq));

        sequenceNumber_ = uranus::utils::readByte<std::uint16_t>(sq);
        sq.clear();

        std::copy_n(buffer.begin() + 4, 4, std::back_inserter(sq));
        timestamp_ = uranus::utils::readByte<std::uint32_t>(sq);
        sq.clear();

        std::copy_n(buffer.begin() + 8, 4, std::back_inserter(sq));
        ssrc_ = uranus::utils::readByte<std::uint32_t>(sq);

        // if (buffer.size() < kFixedHeaderSize + ssrc_ * 4) {
        //    return false;
        //}

        spdlog::info("packet header version:{},padding:{},ext:{},csrc:{},marker:{},pt:{},sq:{},tt:{},ssrc:{}",
                     version_,
                     padding_,
                     extension_,
                     csrc_,
                     marker_,
                     payloadType_,
                     sequenceNumber_,
                     timestamp_,
                     ssrc_);

        payload_ = {};

        return true;
    }
    // Header.
    // -----  1byte (0)
    std::uint16_t version_;  // 2bit 版本号
    bool padding_;           // 1bit 填充位，为1时表示报文末端含有一个或多个填充字节，
                             // 填充位长度不算在负载长度中，且填充部分的最后用来充当计数器
    bool extension_;         // 1bit 扩展， 标识是否含有扩展头（最多只能有一个扩展头）
    std::uint16_t csrc_;     // 4bit CSRC数， RTP头中含有的CRSC的数量

    // ----- 1byte (1)
    bool marker_;                // 1bit 标记，用来标记比特流中重要事件（如帧边界）
    std::uint16_t payloadType_;  // 7bit RTP负载格式，当接受者收到不能识别的PT格式时将忽略该RTP包

    // ----- 2bytes (2,3)
    std::uint16_t sequenceNumber_;  // 16bit 初值随机，每发送一个RTP，序列号加一。可以用来检测丢包和重传

    // 4bytes (4-7)
    std::uint32_t timestamp_;  // 32bit 时间戳反映了RTP数据包中第一个字节的采样时间。

    // 4bytes (8-11)
    std::uint32_t ssrc_;  // 32bit 同步源，即RTP报文的源

    // 4bytes csrc 可选位
    std::vector<std::uint32_t> csrcs_;  // 0~15项，每项 32bit，识别报文中所有的同步源

    std::vector<std::byte> payload_;  // 数据载荷
    std::uint16_t payloadLen_;
};
}  // namespace uranus::rtp
