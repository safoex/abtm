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

#include "../test_defs.h"

using namespace bt;
int main() {

    MemoryJS memory;
    Tree tree(memory, "test");
    Builder builder(&tree);

    LeafJSParser ljsp(builder);

    std::unordered_set<std::string> a;
    bool success = true;

    auto r = ljsp.get_used_vars_from_expr("ab + d");
    a = {"ab","d"};
    success &= r == a;
    REVEAL("SIMPLE")

    r = ljsp.get_used_vars_from_expr("ab.d + c");
    a = {"ab", "c"};
    success &= r == a;
    REVEAL("With 1 member")

    r = ljsp.get_used_vars_from_expr("ab.d.e + c");
    a = {"ab", "c"};
    success &= r == a;
    REVEAL("With 2 members")

    r = ljsp.get_used_vars_from_expr("ab.d[2].e + c[2].e + j");
    a = {"ab", "c","j"};
    success &= r == a;
    REVEAL("With 2 members, num literals")

}