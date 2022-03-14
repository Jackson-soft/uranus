#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
}

namespace uranus::audio {
// 音频合流器
class Mixer {
public:
    auto create() -> bool {
        return true;
    }

    auto addSource() -> bool {
        return true;
    }

    void mix() {}

private:
    auto filters() -> int {
        int             ret{0};
        char            args[512];
        const AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
        const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
        AVFilterInOut  *outputs     = avfilter_inout_alloc();
        AVFilterInOut  *inputs      = avfilter_inout_alloc();

        filter_graph                = avfilter_graph_alloc();

        if (!outputs || !inputs || !filter_graph) {
            return AVERROR(ENOMEM);
        }

        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "in", args, nullptr, filter_graph);
        if (ret < 0) {
            return ret;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out", nullptr, nullptr, filter_graph);
        if (ret < 0) {
            return ret;
        }

        return 0;
    }

    void free() {
        avfilter_graph_free(&filter_graph);
        avcodec_free_context(&dec_ctx);
        avformat_close_input(&fmt_ctx);
        // av_frame_free(&frame);
        // av_frame_free(&filt_frame);
    }

    AVFormatContext               *fmt_ctx;
    AVCodecContext                *dec_ctx;
    AVFilterContext               *buffersink_ctx;
    AVFilterContext               *buffersrc_ctx;
    AVFilterGraph                 *filter_graph;
    int                            audio_stream_index = -1;

    std::vector<AVFilterContext *> srcs_;
};
}  // namespace uranus::audio
