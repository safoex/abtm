cmake_minimum_required(VERSION 3.12)
project(abtm)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(OpenMP REQUIRED) # Find the package


add_executable(abtm test.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp src/TreeParser.cpp)
target_link_libraries(abtm libyaml-cpp.so)

add_executable(speedtest test_speed.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp src/TreeParser.cpp)
target_link_libraries(speedtest libyaml-cpp.so ${OpenMP_CXX_LIBRARIES})

add_executable(testparser src/test_new_parser.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/ActionStrParser.cpp src/parser/ActionStrParser.h src/parser/ExpressionParser.cpp
        src/parser/ExpressionParser.h src/parser/ConditionStrParser.cpp src/parser/ConditionStrParser.h
        src/parser/LeafStrParser.cpp src/parser/LeafStrParser.h src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h)
target_link_libraries(testparser libyaml-cpp.so)

add_executable(test_template_parser src/tests/template_parser/test_template_parser.cpp src/Memory.cpp src/Node.cpp src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/ActionStrParser.cpp src/parser/ActionStrParser.h src/parser/ExpressionParser.cpp
        src/parser/ExpressionParser.h src/parser/ConditionStrParser.cpp src/parser/ConditionStrParser.h
        src/parser/LeafStrParser.cpp src/parser/LeafStrParser.h src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h)
target_link_libraries(test_template_parser libyaml-cpp.so)

add_executable(test_mimo src/tests/mimo/test_mimo.cpp src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/ActionStrParser.cpp src/parser/ActionStrParser.h src/parser/ExpressionParser.cpp
        src/parser/ExpressionParser.h src/parser/ConditionStrParser.cpp src/parser/ConditionStrParser.h
        src/parser/LeafStrParser.cpp src/parser/LeafStrParser.h src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h)
target_link_libraries(test_mimo libyaml-cpp.so)