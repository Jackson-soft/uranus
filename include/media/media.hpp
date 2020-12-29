#pragma once

#include "ffmpeg.hpp"

namespace uranus::media
{
// 音频混音器
class audioMixer
{
public:
    audioMixer()  = default;
    ~audioMixer() = default;

    void init()
    {
        avcodec_register_all();
        av_register_all();
        avformat_network_init();
    }
};
}  // namespace uranus::media