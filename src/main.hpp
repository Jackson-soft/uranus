#pragma once

#include <any>
#include <cstdio>
#include <map>
#include <print>
#include <string>
#include <typeinfo>
#include <vector>

template<typename... Args>
auto Write(Args... args) -> int {
    int num = sizeof...(args);

    std::vector<std::any> vva{args...};

    std::print(stdout, "args number: {}\n", vva.size());
    for (const auto &it : vva) {
        if (it.type() == typeid(int)) {
            std::print(stdout, "int {} true\n", std::any_cast<int>(it));
        } else if (it.type() == typeid(std::string)) {
            std::print(stdout, "string {} true\n", std::any_cast<std::string>(it));
        } else if (it.type() == typeid(double)) {
            std::print(stdout, "double {} true\n", std::any_cast<double>(it));
        } else if (it.type() == typeid(const char *)) {
            std::print(stdout, "char {} true\n", std::any_cast<const char *>(it));
        } else if (it.type() == typeid(std::vector<int>)) {
            std::print(stdout, "vector int {}\n", it.type().name());
            // fmt::print(stdout, "vector {}\n", std::any_cast<std::vector<int>>(it));
        } else if (it.type() == typeid(std::map<std::string, std::string>)) {
            // fmt::print(stdout, "map {} \n", std::any_cast<std::map<std::string,
            // std::string>>(it));
        }
    }

    return num;
}
