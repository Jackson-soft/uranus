#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
}

namespace uranus::audio
{
// 音频编解码器
class Codec
{
    void create() { AVFrame *frame = av_frame_alloc(); }

    void encode() {}

    // 解码
    void decode(const std::vector<std::uint8_t> &data)
    {
        AVPacket *pkt  = av_packet_alloc();
        AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
        if (!codec)
            return;

        AVCodecParserContext *parser = av_parser_init(codec->id);
        AVCodecContext *cctx         = avcodec_alloc_context3(codec);

        avcodec_open2(cctx, codec, nullptr);

        while (true) {
            AVFrame *frame = av_frame_alloc();

            auto ret = av_parser_parse2(
                parser, cctx, &pkt->data, &pkt->size, data.data(), data.size(), AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

            ret = avcodec_send_packet(cctx, pkt);

            ret = avcodec_receive_frame(cctx, frame);

            auto data_size = av_get_bytes_per_sample(cctx->sample_fmt);
        }
    }
};
}  // namespace uranus::audio