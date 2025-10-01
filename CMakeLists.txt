cmake_minimum_required(VERSION 3.15)

project(EchoStrike VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

set(SDL3_DIR ${CMAKE_SOURCE_DIR}/include/SDL3/cmake)
find_package(SDL3 REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/include/pjh_json)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/echo_strike)

add_executable(main ${CMAKE_SOURCE_DIR}/tests/test_vec2.cpp)
target_link_libraries(main PRIVATE echo_strike_lib)