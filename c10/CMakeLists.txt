CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

PROJECT(upp_10)

SET(CMAKE_CXX_STANDARD 23)

FILE(GLOB_RECURSE 01_basics_src 01_basics/*.cpp 01_basics/*.h)
FILE(GLOB_RECURSE 02_advanced_src 02_advanced/*.cpp 02_advanced/*.h)
FILE(GLOB_RECURSE 03_kostra_src 03_kostra/*.cpp 03_kostra/*.h)

ADD_EXECUTABLE(01_basics ${01_basics_src})
ADD_EXECUTABLE(02_advanced ${02_advanced_src})
ADD_EXECUTABLE(03_kostra ${03_kostra_src})

find_package(TBB REQUIRED)
target_link_libraries(01_basics PRIVATE TBB::tbb)
target_link_libraries(02_advanced PRIVATE TBB::tbb)
target_link_libraries(03_kostra PRIVATE TBB::tbb)
