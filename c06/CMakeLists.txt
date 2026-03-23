CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

PROJECT(upp_06)

SET(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

FILE(GLOB_RECURSE 01_reduce_basic_src 01_reduce_basic/*.cpp 01_reduce_basic/*.h)
FILE(GLOB_RECURSE 02_reduce_adv_src 02_reduce_adv/*.cpp 02_reduce_adv/*.h)
FILE(GLOB_RECURSE 03_map_reduce_src 03_map_reduce/*.cpp 03_map_reduce/*.h)

ADD_EXECUTABLE(01_reduce_basic ${01_reduce_basic_src})
ADD_EXECUTABLE(02_reduce_adv ${02_reduce_adv_src})
ADD_EXECUTABLE(03_map_reduce ${03_map_reduce_src})
