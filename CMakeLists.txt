cmake_minimum_required(VERSION 3.20)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GUN")
    add_compile_options(-fcoroutines)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcoroutines-ts)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/std:c++latest /await /W3 /WX)
endif()

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(libmysqlclient REQUIRED)
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
    ${libmysqlclient_INCLUDE_DIRS}
    ${libpqxx_INCLUDE_DIRS})
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE pthread
    ${Boost_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${spdlog_LIBRARIES}
    ${ffmpeg_LIBRARIES}
    ${libmysqlclient_LIBRARIES}
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
