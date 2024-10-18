# minimum cmake version
cmake_minimum_required(VERSION 3.30)

# project name, version and lang
project(demo VERSION 0.1 LANGUAGES CXX)

# compiler options
# -Werror - warnangs as errors
# -Wall - enable all wagnings
add_compile_options(
  -Werror
  -Wall
)

# spec folder for exec target
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

# set lang version - c++23
set(CMAKE_CXX_STANDARD 23)
# set requirement for version
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
# disable lang extensions
set(CMAKE_CXX_EXTENSIONS FALSE)
# set generator
set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
# set path to compiler
set(CMAKE_CXX_COMPILER /usr/bin/gcc)
# set flags for compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++")

add_executable(demo main.cpp)
