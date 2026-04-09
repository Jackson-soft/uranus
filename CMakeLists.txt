cmake_minimum_required(VERSION 3.28)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)

# 主程序
add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE Threads::Threads
            Boost::boost
            nlohmann_json::nlohmann_json
            spdlog::spdlog
            libpqxx::pqxx)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(${PROJECT_NAME} PRIVATE -fexperimental-library)
endif()

# 测试
enable_testing()
find_package(Catch2 REQUIRED)

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)

target_include_directories(${unit_test} PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
    ${unit_test}
    PRIVATE Catch2::Catch2WithMain
            Boost::boost
            nlohmann_json::nlohmann_json
            libpqxx::pqxx)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(${unit_test} PRIVATE -fexperimental-library)
endif()

add_test(NAME ${unit_test} COMMAND $<TARGET_FILE:${unit_test}> --success)
