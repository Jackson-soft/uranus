cmake_minimum_required(VERSION 3.25)

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

find_package(asio REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    ${asio_INCLUDE_DIR}
    ${fmt_INCLUDE_DIR}
    ${nlohmann_json_INCLUDE_DIR}
    ${spdlog_INCLUDE_DIR}
    ${ffmpeg_INCLUDE_DIR}
    ${libmysqlclient_INCLUDE_DIR}
    ${libpqxx_INCLUDES})
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE pthread
    ${asio_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${spdlog_LIBRARIES}
    ${ffmpeg_LIBRARIES}
    ${libmysqlclient_LIBRARIES}
    ${libpqxx_LIBRARIES})

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)
target_include_directories(
    ${unit_test}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    ${Catch2_INCLUDE_DIRS}
    ${asio_INCLUDE_DIRS}
    ${fmt_INCLUDES}
    ${nlohmann_json_INCLUDES}
    ${ffmpeg_INCLUDES}
    ${libmysqlclient_INCLUDE_DIRS}
    ${libpqxx_INCLUDES})
target_link_libraries(
    ${unit_test}
    PRIVATE ${Catch2_LIBRARIES}
    ${asio_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES}
    ${ffmpeg_LIBRARIES}
    ${libmysqlclient_LIBRARIES}
    ${libpqxx_LIBRARIES})

enable_testing()

add_test(NAME catch2 COMMAND $<TARGET_FILE:${unit_test}> --success)
