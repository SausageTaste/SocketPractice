cmake_minimum_required(VERSION 3.11.0)


add_executable(app1
    ./app/app1.cpp
)

target_compile_features(app1 PUBLIC cxx_std_17)
target_include_directories(app1 PUBLIC .)
