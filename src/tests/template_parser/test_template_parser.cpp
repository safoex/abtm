//
// Created by safoex on 16.07.19.
//

#include <parser/Builder.h>
#include "parser/Parsers.h"
#include "Tree.h"
#include "Memory.h"

using namespace bt;

int main() {
    Tree tree("test");
    Builder builder(&tree);
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
            {{"variables", "set"}, new VariablesParser(builder)},
            {{"common"}, new CommonParser(builder)},
    });

    parser.loadYamlFile("../src/tests/template_parser/test_template_parser.yaml");

//    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << builder.get_dot_description(Builder::DOT);
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");

//    tree.start();
//    std::ofstream test_results("output.yaml");
//    test_results << tp.apply_samples("../config/input.yaml");

    std::ofstream test_states("test_states.txt");
    test_states << tree.dot_tree_description(true);
    system("dot -Tpdf test_states.txt > states.pdf");
}