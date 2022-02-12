cmake_minimum_required(VERSION 3.11.0)


add_library(mysocket STATIC
    ./socket_win.cpp
)

target_compile_features(mysocket PUBLIC cxx_std_17)
target_include_directories(mysocket PUBLIC .)

target_link_libraries(mysocket
    PRIVATE
        fmt::fmt
)

if (WIN32)
    target_link_libraries(mysocket
        PUBLIC
            wsock32
            ws2_32
    )
endif()
