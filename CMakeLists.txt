cmake_minimum_required(VERSION 3.16)
project(CO2203_LMS VERSION 1.0 LANGUAGES CXX)

# Same as -std=c++17 in the Makefile
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put the executable in <build>/bin, like the Makefile's bin/ folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)