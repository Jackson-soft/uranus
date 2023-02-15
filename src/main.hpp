#pragma once

#include "fmt/core.h"

#include <any>
#include <cstdio>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

template<typename... Args>
auto Write(Args... args) -> int {
    int num = sizeof...(args);

    std::vector<std::any> vva{args...};

    fmt::print(stdout, "args number: {}\n", vva.size());
    for (const auto &it : vva) {
        if (it.type() == typeid(int)) {
            fmt::print(stdout, "int {} true\n", std::any_cast<int>(it));
        } else if (it.type() == typeid(std::string)) {
            fmt::print(stdout, "string {} true\n", std::any_cast<std::string>(it));
        } else if (it.type() == typeid(double)) {
            fmt::print(stdout, "double {} true\n", std::any_cast<double>(it));
        } else if (it.type() == typeid(const char *)) {
            fmt::print(stdout, "char {} true\n", std::any_cast<const char *>(it));
        } else if (it.type() == typeid(std::vector<int>)) {
            fmt::print(stdout, "vector int {}\n", it.type().name());
            fmt::print(stdout, "vector {}\n", std::any_cast<std::vector<int>>(it));
        } else if (it.type() == typeid(std::map<std::string, std::string>)) {
            fmt::print(stdout, "map {} \n", std::any_cast<std::map<std::string, std::string>>(it));
        }
    }

    return num;
}
