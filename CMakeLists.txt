cmake_minimum_required(VERSION 3.10)
project(korean_3d_text)

# Set build type
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(main main.cpp)
target_compile_options(main PRIVATE -Wall -Wextra)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main raylib)
