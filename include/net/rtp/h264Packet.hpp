#pragma once

namespace aquarius::rtp
{
class h264
{
private:
    /**
     * NaluHeader，占用1个字节，在 RtpHeader 之后。
     * RTP 包含完整包时，RtpPayload = NaluHeader + NaluPayload.
     */
    unsigned int nalu_head_3 : 3; /* 前三位填充 nalu-head 的前三位 */
    unsigned int nalu_type : 5;   /* 后五位表示 nalu-type */
};
}  // namespace aquarius::rtp