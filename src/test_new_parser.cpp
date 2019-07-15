//
// Created by safoex on 15.07.19.
//

#include "parser/Parsers.h"
#include "Tree.h"
#include "Memory.h"

using namespace bt;

int main() {
    Tree tree("test");
    Builder builder(&tree);
    Parser parser(builder, {
        {{"nodes"}, new NodesParser(builder, {
            {{"action"}, new ActionStrParser(builder)},
            {{"condition"}, new ConditionStrParser(builder)},
            {{"sequence", "selector", "skipper", "parallel"}, new ControlNodeParser(builder)}
       })},
        {{"variables", "set"}, new VariablesParser(builder)},
        {{"common"}, new CommonParser(builder)},
    });

    parser.loadYamlFile("../config/test.yaml");
    builder.make_graph();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << tree.dot_tree_description();
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");

//    tree.start();
//    std::ofstream test_results("output.yaml");
//    test_results << tp.apply_samples("../config/input.yaml");

    std::ofstream test_states("test_states.txt");
    test_states << tree.dot_tree_description(true);
    system("dot -Tpdf test_states.txt > states.pdf");
}