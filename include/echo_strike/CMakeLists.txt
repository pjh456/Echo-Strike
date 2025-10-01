cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "E:/Programs/libsdl/SDL;E:/Programs/libsdl/SDL-image;E:/Programs/libsdl/SDL-ttf;")
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3 Headers SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

file(GLOB_RECURSE echo_strike_header ${CMAKE_CURRENT_SOURCE_DIR}/*/*.hpp)
file(GLOB_RECURSE echo_strike_source ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)

add_library(echo_strike_lib STATIC ${echo_strike_header} ${echo_strike_source})

target_include_directories(echo_strike_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(echo_strike_lib
    PRIVATE pjh_json
    PRIVATE SDL3::SDL3
    PRIVATE SDL3_image::SDL3_image
    PRIVATE SDL3_ttf::SDL3_ttf)