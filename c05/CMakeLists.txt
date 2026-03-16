CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

PROJECT(upp_05)

SET(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

FILE(GLOB_RECURSE 01_doall_src 01_doall/*.cpp 01_doall/*.h)
FILE(GLOB_RECURSE 02_dopipe_src 02_dopipe/*.cpp 02_dopipe/*.h)
FILE(GLOB_RECURSE 03_dynamic_src 03_dynamic/*.cpp 03_dynamic/*.h)

ADD_EXECUTABLE(01_doall ${01_doall_src})
ADD_EXECUTABLE(02_dopipe ${02_dopipe_src})
ADD_EXECUTABLE(03_dynamic ${03_dynamic_src})
