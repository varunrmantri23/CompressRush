cmake_minimum_required(VERSION 3.20)
project(CompressRush VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files here
add_executable(compressrush 
    src/main.cpp
    src/algorithms/huffman.cpp
    src/algorithms/lzw.cpp
)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()