cmake_minimum_required(VERSION 3.15)

project(EchoStrike VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -O2")

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(SDL_TEST_LIBRARY OFF)
set(CMAKE_PREFIX_PATH E:/Programs/SDL-static)

# add_subdirectory(include/SDL EXCLUDE_FROM_ALL)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3 Headers SDL3-shared)

# find_package(SDL3 REQUIRED CONFIG)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/pjh_json)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/echo_strike)

add_executable(main ${CMAKE_SOURCE_DIR}/tests/test_sdl.cpp)
target_link_libraries(main PRIVATE echo_strike_lib SDL3::SDL3)

if(WIN32)
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:main>
        VERBATIM
    )
endif()