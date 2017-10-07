
cmake_minimum_required(VERSION 3.6)

project(pass)

find_package(LLVM REQUIRED CONFIG)

message(${LLVM_VERSION})
message(${LLVM_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-g")

set(CMAKE_CXX_STANDARD 14)


add_library(p1 MODULE pass.cc)

