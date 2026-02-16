cmake_minimum_required(VERSION 3.16)

# using with Neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(upp_sp1 LANGUAGES CXX)

# C++ version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# collect all .cpp files
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
)

# main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# strict debug
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /W4                 # Baseline strict
            /w14242 /w14265     # Specific useful high-level warnings
            /w14287 /w14296
            /w14311 /w14826
            /permissive-        # Standards conformance
            /WX                 # Warnings as errors
        >
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            -Wall -Wextra       # Reasonable strictness
            -Wpedantic          # ISO C++ conformance
            -Wshadow            # Local variable shadows another
            -Wnon-virtual-dtor  # Missing virtual destructor
            -Wold-style-cast    # C-style casts in C++
            -Wcast-align        # Potential alignment issues
            -Wunused            # Unused variables/functions
            -Woverloaded-virtual # Hiding virtual functions
            -Wconversion        # Implicit type conversions that lose data
            -Wsign-conversion   # Signed/unsigned comparison/conversion
            -Wnull-dereference  # Detect potential null pointer usage
            -Wdouble-promotion  # Float to double promotion
            -Wformat=2          # Strict printf-style check
            # -Werror             # Warnings as errors
        >
    )
endif()

# set binaries dir for output
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

# symlink compile_commands.json from build to source dir (if it exists)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
        COMMENT "Symlinking compile_commands.json to source root"
        VERBATIM
    )
endif()
