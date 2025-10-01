cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE echo_strike_header ${CMAKE_CURRENT_SOURCE_DIR}/*/*.hpp)
file(GLOB_RECURSE echo_strike_source ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)

add_library(echo_strike_lib STATIC ${echo_strike_header} ${echo_strike_source})

target_include_directories(echo_strike_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(echo_strike_lib
    PRIVATE pjh_json
    PRIVATE SDL3::SDL3)