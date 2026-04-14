cmake_minimum_required(VERSION 3.12)
project(upp_08)

# using w/ neovim
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# modern C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMP
find_package(OpenMP REQUIRED)

# avoid repetition
function(add_task name folder)
    file(GLOB_RECURSE SRC ${folder}/*.cpp ${folder}/*.h)
    add_executable(${name} ${SRC})
    target_link_libraries(${name} PRIVATE OpenMP::OpenMP_CXX)
endfunction()

# tasks
add_task(parallel_for      01_parallel_for)
add_task(parallel_reduce   02_parallel_reduce)
add_task(barriers          03_barriers)
add_task(tree              04_tree)
add_task(bonus             05_bonus)
