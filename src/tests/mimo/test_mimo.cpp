//
// Created by safoex on 23.07.19.
//

#include <parser/Builder.h>
#include "parser/Parsers.h"
#include "Tree.h"
#include "Memory.h"
#include "cpp_function_examples.h"
#include "io/MIMOCenter.h"
#include "io/cpp/CPPFunctionParser.h"
#include "icub_function_example.h"
#include "parser/Scope.h"

using namespace bt;

int main() {
    Network yarp;

    // create behavior tree
    Tree tree("test");

    // create builder for tree
    Builder builder(&tree);

    // create input output manager
    MIMOCenter mimo(&tree);
    iCubRobotDemo icub_test;
    // create parsers:
    auto cp = new CPPFunctionParser(builder, mimo, {
            examples::functions,
            icub_test.functions()
    });
    auto np = new NodesParser(builder, {
            {{"action"}, new ActionStrParser(builder)},
            {{"condition"}, new ConditionStrParser(builder)},
            {{"sequence", "selector", "skipper", "parallel"}, new ControlNodeParser(builder)}
    });
    auto tp = new TemplateParser(builder, np);
    np->registerModule("template", tp);
    Parser parser(builder, {
            {{"nodes"}, np},
            {{"templates"}, tp},
            {{"variables"}, new VariablesParser(builder)},
            {{"set"}, new SetVariablesParser(builder)},
            {{"common"}, new CommonParser(builder)},
            {{"functions"}, new Scope(builder, {
                    {{"cpp"}, cp}
            })}
    });

    parser.loadYamlFile("../src/tests/mimo/test_mimo.yaml");

//    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << builder.get_dot_description(Builder::DOT);
//    test_gv << tree.dot_tree_description(false);
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");

//    tree.start();
//    std::ofstream test_results("output.yaml");
//    test_results << tp.apply_samples("../config/input.yaml");

    tree.get_memory().var["time"] = Time::now();
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
        mimo.process({{"time", Time::now()}}, INPUT);
        auto new_desc = builder.get_dot_description(Builder::DOT_STATES);
//        new_desc = tree.dot_tree_description(true);
        if(new_desc != desc) {
            desc = new_desc;
            test_states << desc;
            system("dot -Tpdf test_states.txt > states.pdf");
//            Bottle& output = outPort.prepare();
//            output.clear();
//            output.addString(desc);
//            outPort.write();
        }
    }


}