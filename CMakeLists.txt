cmake_minimum_required(VERSION 3.15)

project(uranus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GUN")
    add_compile_options(-fcoroutines)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcoroutines-ts)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/std:c++latest /await /W3 /WX)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(Boost REQUIRED COMPONENTS)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
find_package(ffmpeg REQUIRED)
find_package(MySQL REQUIRED)
find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
            ${Boost_INCLUDES}
            ${fmt_INCLUDES}
            ${spdlog_INCLUDES}
            ${ffmpeg_INCLUDES}
            ${MySQL_INCLUDES}
            ${libpqxx_INCLUDES})
target_link_libraries(
    ${PROJECT_NAME} PRIVATE pthread ${Boost_LIBRARIES} ${ffmpeg_LIBRARIES}
                            ${MySQL_LIBRARIES} ${libpqxx_LIBRARIES} ${fmt_LIBS})

set(unit_test my_test)
add_executable(${unit_test} test/main.cpp)
target_include_directories(
    ${unit_test}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
            ${Boost_INCLUDES}
            ${Catch2_INCLUDES}
            ${fmt_INCLUDES}
            ${spdlog_INCLUDES}
            ${MySQL_INCLUDES}
            ${libpqxx_INCLUDES})
target_link_libraries(
    ${unit_test} PRIVATE ${Boost_LIBRARIES} ${MySQL_LIBRARIES} ${fmt_LIBS}
                         ${libpqxx_LIBRARIES} ${Catch2_LIBRARIES})

enable_testing()

add_test(NAME catch2 COMMAND $<TARGET_FILE:${unit_test}> --success)
