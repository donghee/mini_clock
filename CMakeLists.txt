cmake_minimum_required(VERSION 3.10)
project(korean_3d_text)

# Set static analyzer (disabled for now)
# find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
# if(CMAKE_CXX_CPPCHECK)
#   set(cppcheck_opts "--enable=all" "--inline-suppr" "--quiet"
#     "--suppressions-list=${PROJECT_SOURCE_DIR}/.cppcheck.supp"
#     # "--checkers-report=${PROJECT_BINARY_DIR}/cppcheck.report"
#   )
#   list(APPEND CMAKE_C_CPPCHECK "--std=c11" ${cppcheck_opts})
# 	list(APPEND CMAKE_CXX_CPPCHECK "--std=c++20" ${cppcheck_opts})
# endif()

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
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(main main.cpp)
target_compile_options(main PRIVATE -Wall -Wextra)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main raylib)

# Copy font file to build directory if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/NanumGothic.ttf")
    configure_file("${CMAKE_SOURCE_DIR}/NanumGothic.ttf" "${CMAKE_BINARY_DIR}/NanumGothic.ttf" COPYONLY)
endif()
