#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
}

namespace uranus::media::codec
{
class video
{
public:
    video()  = default;
    ~video() = default;

    auto create() -> bool { return true; }
};
}  // namespace uranus::media::codec