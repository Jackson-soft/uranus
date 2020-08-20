cmake_minimum_required(VERSION 3.13)

project(uranus CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} spdlog::spdlog yaml-cpp pq)

set(test unit)
add_executable(${test} test/main.cpp)
target_link_libraries(${test} PRIVATE ${Boost_LIBRARIES} spdlog::spdlog yaml-cpp pq)
