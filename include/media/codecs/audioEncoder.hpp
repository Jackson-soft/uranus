#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace uranus::media::codec
{
class audioEncoder
{
public:
    audioEncoder()  = default;
    ~audioEncoder() = default;
};
}  // namespace uranus::media::codec