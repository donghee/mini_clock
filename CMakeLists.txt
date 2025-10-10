cmake_minimum_required(VERSION 3.10)
project(mini_clock)

# Set build type
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    # Enable PIC for raylib to allow linking with shared library
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Clock library (shared library for hot reload)
# Note: Does NOT link raylib - uses symbols from main executable
add_library(clock SHARED clock_lib.cpp)
target_compile_options(clock PRIVATE -Wall -Wextra)
target_compile_features(clock PRIVATE cxx_std_20)
target_include_directories(clock PRIVATE ${raylib_SOURCE_DIR}/src)
set_target_properties(clock PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    PREFIX ""
    LINK_FLAGS "-Wl,--unresolved-symbols=ignore-all"
)

# Main executable
add_executable(main main.cpp)
target_compile_options(main PRIVATE -Wall -Wextra)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main raylib ${CMAKE_DL_LIBS})
# Export symbols for the shared library to use
set_target_properties(main PROPERTIES
    ENABLE_EXPORTS TRUE
)

# Copy the library to the output directory after build
add_custom_command(TARGET clock POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:clock>
    ${CMAKE_BINARY_DIR}/libclock.so
    COMMENT "Copying clock library to build directory"
)
