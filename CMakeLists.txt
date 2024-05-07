cmake_minimum_required(VERSION 3.25)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 启用无栈协程，须 gcc 10以上
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring on/for Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring on/for macOS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -fexperimental-library")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on/for Windows")
else()
    message(STATUS "Configuring on/for ${CMAKE_SYSTEM_NAME}")
endif()

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${ffmpeg_INCLUDE_DIRS}
    ${libpqxx_INCLUDE_DIRS})
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE pthread
    ${Boost_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${spdlog_LIBRARIES}
    ${ffmpeg_LIBRARIES}
    ${libpqxx_LIBRARIES})

enable_testing()

find_package(Catch2 REQUIRED)

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)
target_include_directories(
    ${unit_test}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    ${Catch2_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${fmt_INCLUDE_DIRS}
    ${nlohmann_json_INCLUDE_DIRS}
    ${ffmpeg_INCLUDE_DIRS}
    ${libmysqlclient_INCLUDE_DIRS}
    ${libpqxx_INCLUDE_DIRS})

target_link_libraries(
    ${unit_test}
    PRIVATE ${Catch2_LIBRARIES}
    ${Boost_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${ffmpeg_LIBRARIES}
    ${libmysqlclient_LIBRARIES}
    ${libpqxx_LIBRARIES})

add_test(NAME ${unit_test} COMMAND $<TARGET_FILE:${unit_test}> --success)
