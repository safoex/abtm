cmake_minimum_required(VERSION 3.12)
project(abtm)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_executable(abtm test.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp src/TreeParser.cpp)
target_link_libraries(abtm libyaml-cpp.so)