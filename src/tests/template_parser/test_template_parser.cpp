//
// Created by safoex on 16.07.19.
//

#include <parser/Builder.h>
#include "parser/Parsers.h"
#include "Tree.h"
#include "memory/MemoryJS.h"
#include "io/MIMOCenter.h"

using namespace bt;

int main() {
    MemoryJS memory;
    Tree tree(memory, "test");
    Builder builder(&tree);


    MIMOCenter mimo(&tree);


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
            {{"common"}, new CommonParser(builder)}
    });
    auto tp = new TemplateParser(builder, np, &parser);
    np->registerModule("template", tp);

    parser.registerModule("templates", tp);
    parser.loadYamlFile("../src/tests/template_parser/test_template_parser.yaml");

//    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << builder.get_dot_description(Builder::DOT) << std::endl;
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");

    {
        std::ofstream test_gv2("test_gv_tree_expanded.txt");
        test_gv2 << tree.dot_tree_description() << std::endl;
        system("dot -Tpdf test_gv_tree_expanded.txt > tree_expanded.pdf");
    }

//    tree.start();
//    std::ofstream test_results("output.yaml");
//    test_results << tp.apply_samples("../config/input.yaml");

    std::ofstream test_states("test_states.txt");
    test_states << tree.dot_tree_description(true) << std::endl;
    system("dot -Tpdf test_states.txt > states.pdf");
}