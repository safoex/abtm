//
// Created by safoex on 05.08.19.
//

#include "defs.h"

#include <yaml-cpp/yaml.h>
#include "Tree.h"
#include "parser/Parsers.h"
#include "memory/MemoryJS.h"
#include "io/MIMOCenter.h"
#include "io/cpp/CPPFunctionParser.h"
#include "../../mimo/cpp_function_examples.h"
#include "../../mimo/icub_function_example.h"
#include <iostream>

using namespace bt;
int main() {

    MemoryJS memory;
    Tree tree(memory, "test");
    Builder builder(&tree);


    MIMOCenter mimo(&tree);
    iCubRobotDemo icub_test;

    auto cp = new CPPFunctionParser(builder, mimo, {
            examples::functions
            ,icub_test.functions()
    });

    auto vinp = new VariablesInNodeParser(builder);

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
            {{"functions"}, new Scope(builder, {
                    {{"cpp"}, cp}
            })}
    });
    auto tp = new TemplateParser(builder, np, &parser);
    np->registerModule("template", tp);

    parser.registerModule("templates", tp);

    parser.loadYamlFile("../src/tests/unit/tree/test_leaf2.yaml");

//    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << builder.get_dot_description(Builder::DOT) << std::endl;
    test_gv << tree.dot_tree_description(false);
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");

    tree.get_memory().set("time", Time::now());
    mimo.start();

    BufferedPort<Bottle> outPort;
    bool ok = outPort.open("/hello/out");
    if (!ok) {
        fprintf(stderr, "Failed to create ports.\n");
        fprintf(stderr, "Maybe you need to start a nameserver (run 'yarpserver')\n");
        return 1;
    }

    std::string desc;
    std::ofstream test_states("test_states.txt");

    while(true) {
        Time::delay(1);
        mimo.process({{"time", Time::now()}}, MIMO_INPUT);
        auto new_desc = builder.get_dot_description(Builder::DOT_STATES);
        new_desc = tree.dot_tree_description(true);
        if(new_desc != desc) {
            desc = new_desc;
            test_states << desc << std::endl;
            system("dot -Tpdf test_states.txt > states.pdf");
        }
        std::cout << "------------- CHECK values -----------" << std::endl;
        for(auto v: {"__function_icub_move_return"})
            std::cout << v << ": " << tree.get_memory().get_string(v).value() << std::endl;
    }




}