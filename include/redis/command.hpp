#pragma once

#include <any>
#include <vector>

namespace uranus::redis {
// redis 命令的基类
class BaseCmd {
public:
    BaseCmd()          = default;
    virtual ~BaseCmd() = default;

    template<typename T>
    explicit BaseCmd(T &&args...) : args_(args) {}

    auto Args() -> std::vector<std::any> {
        return args_;
    }

private:
    std::vector<std::any> args_;
};

class StringCmd : public BaseCmd {
public:
    auto Result() -> const std::string & {
        return value_;
    }

private:
    std::string value_;
};

class SliceCmd : public BaseCmd {
public:
    auto Result() -> std::vector<std::any> {
        return value_;
    }

private:
    void readReply() {}

    std::vector<std::any> value_;
};
}  // namespace uranus::redis
