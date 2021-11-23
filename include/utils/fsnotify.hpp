#pragma once

#include <fcntl.h>
#include <string_view>
#include <sys/fanotify.h>
#include <unistd.h>

// 文件监控

namespace uranus::utils {
class FsNotify {
public:
    FsNotify() {}

    auto Add() -> bool
    {
        return false;
    }

    auto Remove() -> bool
    {
        return false;
    }

    void Run()
    {
        char buf[256];
        auto len = ::read(fd_, buf, sizeof(buf));

        if (len == -1) {
            return;
        }
    }

private:
    auto init() -> bool
    {
        fd_ = ::fanotify_init(FAN_CLOEXEC | FAN_CLASS_NOTIF | FAN_NONBLOCK, 0);
        return fd_ > 0;
    }

    auto mark(std::string_view file) -> bool
    {
        auto ret = ::fanotify_mark(fd_, FAN_MARK_ADD, FAN_CLOSE_WRITE, AT_FDCWD, file.data());

        return ret > 0;
    }

private:
    int fd_;
};
}  // namespace uranus::utils
