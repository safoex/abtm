cmake_minimum_required(VERSION 3.12)
project(abtm)

set(CMAKE_CXX_STANDARD 17)


include_directories(src ../../libs/ChaiScript/include ../../libs/duktape-2.4.0/src ../../libs/duktape-2.4.0/extras/logging)
include_directories(../../libs/rosbridgecpp/rapidjson/include)
include_directories(../../libs/esprima-cpp)
include_directories(../../libs/rosbridgecpp)
include_directories(../../libs/openGA/src/)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Simple-WebSocket-Server") #PROJECT_SOURCE_DIR
add_subdirectory(Simple-WebSocket-Server)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

add_library(rosbridgecpp INTERFACE)
target_include_directories(rosbridgecpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rosbridgecpp INTERFACE simple-websocket-server)

find_package(OpenMP REQUIRED) # Find the package
find_package(YARP REQUIRED)
find_package(OpenCV REQUIRED)
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

add_executable(test_template_parser src/tests/template_parser/test_template_parser.cpp ../../libs/duktape-2.4.0/src/duktape.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp
        src/io/offline_tests/OfflineTestChannel.cpp)
target_link_libraries(test_template_parser libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} pthread dl m)

add_executable(test_mimo src/tests/mimo/test_mimo.cpp src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/ActionStrParser.cpp src/parser/ActionStrParser.h src/parser/ExpressionParser.cpp
        src/parser/ExpressionParser.h src/parser/ConditionStrParser.cpp src/parser/ConditionStrParser.h
        src/parser/LeafStrParser.cpp src/parser/LeafStrParser.h src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h)

target_link_libraries(test_mimo libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})

add_executable(test_icub src/tests/mimo/test_icub.cpp src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h)
target_link_libraries(test_icub libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})

add_executable(test_js src/tests/embedjs/testjs.cpp ../../libs/duktape-2.4.0/src/duktape.c src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h )

target_link_libraries(test_js pthread dl m)


add_executable(memory_unit_test src/tests/unit/memory/memory_js.cpp ../../libs/duktape-2.4.0/src/duktape.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h)

target_link_libraries(memory_unit_test pthread dl m)

add_executable(tree_unit_test src/tests/unit/tree/tree_unit_test.cpp ../../libs/duktape-2.4.0/src/duktape.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
         src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp)

target_link_libraries(tree_unit_test libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} pthread dl m)


add_executable(test_used_vars src/tests/unit/used_vars/test_used_vars.cpp ../../libs/duktape-2.4.0/src/duktape.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp)

target_link_libraries(test_used_vars libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} pthread dl m)

add_executable(test_offline src/tests/offline/test_offline.cpp ../../libs/duktape-2.4.0/src/duktape.c ../../libs/duktape-2.4.0/extras/logging/duk_logging.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp
        src/io/offline_tests/OfflineTestChannel.cpp)

target_link_libraries(test_offline libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} pthread dl m)

add_executable(test_misc src/tests/misc/test.cpp)
target_link_libraries(test_misc libyaml-cpp.so)


add_executable(test_ros src/tests/ros/test_ros.cpp ../../libs/duktape-2.4.0/src/duktape.c ../../libs/duktape-2.4.0/extras/logging/duk_logging.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h src/tests/mimo/icub_function_example.h src/tests/mimo/iCubRobot/iCubRobot.cpp
        src/tests/mimo/iCubRobot/iCubRobot.h src/tests/mimo/iCubRobot/iCubPart.cpp src/tests/mimo/iCubRobot/iCubPart.h
        src/tests/mimo/iCubRobot/iCubEmotions.cpp src/tests/mimo/iCubRobot/iCubEmotions.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp
        src/io/offline_tests/OfflineTestChannel.cpp src/io/ros/ROSParser.cpp ../../libs/rosbridgecpp/rosbridge_ws_client.hpp
        src/io/ros/ROSTopic.cpp )

target_link_libraries(test_ros libyaml-cpp.so ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} pthread dl m boost_system rosbridgecpp)


add_executable(ga src/ga/ga.cpp ../../libs/duktape-2.4.0/src/duktape.c ../../libs/duktape-2.4.0/extras/logging/duk_logging.c
        src/memory/MemoryBase.cpp src/memory/MemoryJS.cpp src/memory/MemoryJS.h src/Memory.cpp src/Node.cpp src/ExecutorBase.h src/Tree.cpp
        src/parser/Parser.cpp src/parser/Parser.h src/parser/Builder.cpp  src/parser/BaseParser.cpp src/parser/BaseParser.h
        src/parser/ControlNodeParser.cpp src/parser/NodeParser.cpp src/parser/NodeParser.h
        src/parser/CommonParser.cpp
        src/parser/CommonParser.h src/parser/NodesParser.cpp src/parser/NodesParser.h src/parser/ParserWithModules.cpp
        src/parser/ParserWithModules.h src/parser/VariablesParser.cpp src/parser/VariablesParser.h
        src/parser/TemplateParser.cpp src/parser/TemplateParser.h  src/io/IOBase.cpp
        src/io/MIMOCenter.cpp src/io/MIMOCenter.h src/io/cpp/SynchronousAction.cpp src/io/cpp/SynchronousAction.h
        src/io/cpp/CPPFunctionParser.cpp src/io/cpp/CPPFunctionParser.h src/tests/mimo/cpp_function_examples.h
        src/io/cpp/CPPFunction.cpp src/io/cpp/CPPFunction.h src/io/IOParser.cpp src/io/IOParser.h src/parser/Scope.cpp
        src/parser/Scope.h
        src/parser/js/ActionJSParser.cpp src/parser/js/ConditionJSParser.cpp ../../libs/esprima-cpp/esprima.cpp
        src/io/offline_tests/OfflineTestChannel.cpp)

target_link_libraries(ga libyaml-cpp.so pthread dl m)
