cmake_minimum_required(VERSION 3.11.0)


# Import libraris
#-------------------------------------------------------------------------------

add_subdirectory(./lib)


# App - App 1
#-------------------------------------------------------------------------------

add_executable(app1
    ./app/app1.cpp
)

target_compile_features(app1 PUBLIC cxx_std_17)
target_include_directories(app1 PUBLIC .)

if (WIN32)
    target_link_libraries(app1
        mysocket
    )
endif()


# App - Server 1
#-------------------------------------------------------------------------------

add_executable(server1
    ./app/server1.cpp
)

target_compile_features(server1 PUBLIC cxx_std_17)
target_include_directories(server1 PUBLIC .)

if (WIN32)
    target_link_libraries(server1
        mysocket
    )
endif()
