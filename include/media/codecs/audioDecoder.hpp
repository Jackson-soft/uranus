#pragma once

#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace uranus::media::codec
{
// 音频解码器
class audioDecoder
{
public:
    audioDecoder()  = default;
    ~audioDecoder() = default;

    void init() { pkt = av_packet_alloc(); }

private:
    AVCodec *codec;
    AVCodecContext *cctx;
    AVPacket *pkt;
    AVCodecParserContext *parser;
};
}  // namespace uranus::media::codec