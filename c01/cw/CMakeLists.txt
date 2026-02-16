CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

PROJECT(upp_01)

SET(CMAKE_CXX_STANDARD 20)

ADD_EXECUTABLE(upp_01_a a/main.cpp)
ADD_EXECUTABLE(upp_01_b b/main.cpp)
ADD_EXECUTABLE(upp_01_c c/main.cpp)
ADD_EXECUTABLE(upp_01_d d/main.cpp)
ADD_EXECUTABLE(upp_01_dcv domaci_ukol/main.cpp)

find_package(TBB REQUIRED)

target_link_libraries(upp_01_c PRIVATE TBB::tbb)

# set binaries dir for output
set_target_properties(upp_01_a upp_01_b upp_01_c upp_01_d upp_01_dcv PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# symlink compile_commands.json from build to source dir (if it exists)
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${PROJECT_SOURCE_DIR}/compile_commands.json"
    )
endif()
