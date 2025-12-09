cmake_minimum_required(VERSION 3.15)
project(Lab4_Multithreading)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(lab4
    src/main.cpp
    src/task1.cpp
    src/task2.cpp
    src/task3.cpp
)

target_link_libraries(lab4 pthread)
