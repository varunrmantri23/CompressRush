cmake_minimum_required(VERSION 3.20)
project(CompressRush VERSION 0.1.0)

# C++ standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/algorithms/huffman.cpp
    src/algorithms/lzw.cpp
)

# Create main executable
add_executable(compressrush ${SOURCES})

# Compiler warnings
if(MSVC)
    target_compile_options(compressrush PRIVATE /W4)
else()
    target_compile_options(compressrush PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add include directories
target_include_directories(compressrush PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)