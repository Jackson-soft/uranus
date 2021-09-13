#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace uranus::video {
// 视频编解码器
class Codec {
public:
    auto create() -> bool
    {
        return true;
    }
};
}  // namespace uranus::video
