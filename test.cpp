//
// Created by safoex on 02.04.19.
//

#include <yaml-cpp/yaml.h>
#include <Tree.h>
#include <TreeParser.h>

using namespace bt;

int main() {
    Tree t("test");
    TreeParser tp(&t, "../config/test.yaml");
    tp.load();

    std::ofstream test_gv("test_gv_tree.txt");
    test_gv << t.dot_tree_description();

    t.start();
    std::ofstream test_results("output.yaml");
    test_results << tp.apply_samples("../config/input.yaml");

    std::ofstream test_states("test_states.txt");
    test_states << t.dot_tree_description(true);

}