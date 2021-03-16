cmake_minimum_required(VERSION 3.15)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 启用无栈协程，须 gcc 10以上
set(CMAKE_CXX_FLAGS "-fcoroutines-ts")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR})

find_package(Boost REQUIRED COMPONENTS)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(doctest REQUIRED)
find_package(
  FFmpeg
  COMPONENTS AVCODEC
             AVFORMAT
             AVUTIL
             AVDEVICE
             AVFILTER
             SWSCALE
             SWRESAMPLE
  REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDES}
                          ${spdlog_INCLUDES} ${yaml-cpp_INCLUDES} ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} pthread ${FFMPEG_LIBRARIES})

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)
target_include_directories(
  ${unit_test} PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDES}
                       ${doctest_INCLUDES} ${spdlog_INCLUDES})
target_link_libraries(${unit_test} PRIVATE ${Boost_LIBRARIES})

enable_testing()

add_test(NAME doctest COMMAND $<TARGET_FILE:${unit_test}> --success)
