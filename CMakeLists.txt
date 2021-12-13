cmake_minimum_required(VERSION 3.15)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用无栈协程，须 gcc 10以上
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring on/for Linux")
    set(CMAKE_CXX_FLAGS "-fcoroutines")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring on/for macOS")
    set(CMAKE_CXX_FLAGS "-fcoroutines-ts")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on/for Windows")
else()
    message(STATUS "Configuring on/for ${CMAKE_SYSTEM_NAME}")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(Boost REQUIRED COMPONENTS)
find_package(spdlog REQUIRED)
find_package(doctest REQUIRED)
find_package(libpqxx REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(hiredis REQUIRED)
find_package(MySQL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDES} ${spdlog_INCLUDES}
            ${ffmpeg_INCLUDES} ${hiredis_INCLUDES} ${MySQL_INCLUDES})
target_link_libraries(
    ${PROJECT_NAME} PRIVATE pthread ${Boost_LIBRARIES} ${ffmpeg_LIBRARIES}
                            ${hiredis_LIBRARIES} ${MySQL_LIBRARIES})

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)
target_include_directories(
    ${unit_test}
    PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDES} ${doctest_INCLUDES}
            ${spdlog_INCLUDES} ${hiredis_INCLUDES} ${MySQL_INCLUDES})
target_link_libraries(
    ${unit_test} PRIVATE ${Boost_LIBRARIES} ${hiredis_LIBRARIES}
                         ${MySQL_LIBRARIES})

enable_testing()

add_test(NAME doctest COMMAND $<TARGET_FILE:${unit_test}> --success)
