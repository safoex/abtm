//
// Created by safoex on 09.08.19.
//


#include "defs.h"

#include <yaml-cpp/yaml.h>
#include "Tree.h"
#include "parser/Parsers.h"
#include "memory/MemoryJS.h"
#include "io/MIMOCenter.h"
#include "io/cpp/CPPFunctionParser.h"
#include "io/offline_tests/OfflineTestChannel.h"
#include "io/ros/ROSParser.h"
#include <iostream>

using namespace bt;
int main() {

    MemoryJS memory;
    Tree tree(memory, "test");
    Builder builder(&tree);


    MIMOCenter mimo(&tree);


    auto vinp = new VariablesInNodeParser(builder);
    auto rosp = new ROSParser(builder, mimo);

    auto np = new NodesParser(builder, {
            {{"action"}, new ActionJSParser(builder, vinp)},
            {{"condition"}, new ConditionJSParser(builder, vinp)},
            {{"sequence", "selector", "skipper", "parallel"}, new ControlNodeParser(builder)}
    });
    Parser parser(builder, {
            {{"add"}, vinp},
            {{"nodes"}, np},
            {{"variables"}, new VariablesParser(builder)},
            {{"set"}, new SetVariablesParser(builder)},
            {{"common"}, new CommonParser(builder)},
            {{"ROS"}, rosp}
    });
    auto tp = new TemplateParser(builder, np, &parser);
    np->registerModule("template", tp);

    parser.registerModule("templates", tp);


    std::string TEST = "1";

    parser.loadYamlFile("../src/tests/ros/" + TEST +"/test.yaml");


//    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
//    test_gv << builder.get_dot_description(Builder::DOT) << std::endl;
    test_gv << tree.dot_tree_description(false) << std::endl;
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");



    mimo.start();

    while(1) {
        this_thread::sleep_for(100ms);
        std::ofstream test_states("test_states.txt");
        test_states << tree.dot_tree_description(true);
        system("dot -Tpdf test_states.txt > states.pdf");
    }

}