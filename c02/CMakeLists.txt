CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

PROJECT(upp_02)

FILE(GLOB_RECURSE manuf_files manufactory/*.cpp manufactory/*.h)

ADD_EXECUTABLE(manufactory ${manuf_files})
ADD_EXECUTABLE(sdilene_promenne sdilene_promenne/main.cpp)
