cmake_minimum_required(VERSION 3.15)

project(EchoStrike VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -O2")

set(CMAKE_PREFIX_PATH "E:/Programs/libsdl/SDL;E:/Programs/libsdl/SDL-image;E:/Programs/libsdl/SDL-ttf;")
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3 Headers SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SDL_TEST_LIBRARY OFF)

# add_subdirectory(include/SDL EXCLUDE_FROM_ALL)

# add_subdirectory(${CMAKE_SOURCE_DIR}/include/SDL_image EXCLUDE_FROM_ALL)

# find_package(SDL3 REQUIRED CONFIG)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/pjh_json)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/echo_strike)

add_executable(main ${CMAKE_SOURCE_DIR}/tests/test_quadtree.cpp)
target_link_libraries(main
    PRIVATE echo_strike_lib
    PRIVATE SDL3::SDL3
    PRIVATE SDL3_image::SDL3_image
    PRIVATE SDL3_ttf::SDL3_ttf)

if(WIN32)
add_custom_command(
        TARGET main POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:main>
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_ttf::SDL3_ttf> $<TARGET_FILE_DIR:main>
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_image::SDL3_image> $<TARGET_FILE_DIR:main>
        VERBATIM
    )
endif()