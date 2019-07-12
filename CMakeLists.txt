cmake_minimum_required(VERSION 3.12)
project(abtm)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(OpenMP REQUIRED) # Find the package


add_executable(abtm test.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp src/TreeParser.cpp)
target_link_libraries(abtm libyaml-cpp.so)

add_executable(speedtest test_speed.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp src/TreeParser.cpp
        src/parser/Parser.cpp src/parser/Builder.cpp  src/parser/BaseParser.cpp
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h src/parser/ActionStrParser.cpp src/parser/ActionStrParser.h src/parser/ExpressionParser.cpp src/parser/ExpressionParser.h src/parser/ConditionStrParser.cpp src/parser/ConditionStrParser.h src/parser/LeafStrParser.cpp src/parser/LeafStrParser.h)
target_link_libraries(speedtest libyaml-cpp.so ${OpenMP_CXX_LIBRARIES})