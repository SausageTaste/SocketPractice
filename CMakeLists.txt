cmake_minimum_required(VERSION 3.11.0)

include(FetchContent)


# Import libraris
#-------------------------------------------------------------------------------

FetchContent_Declare(
    fetch_fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        8.1.1
)

FetchContent_MakeAvailable(fetch_fmt)


# Subdirectories
#-------------------------------------------------------------------------------

add_subdirectory(./lib)
add_subdirectory(./app)
