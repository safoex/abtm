//
// Created by safoex on 03.08.19.
//

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include "defs.h"
#include "memory/MemoryJS.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

using namespace bt;
using namespace rapidjson;

#include "../test_defs.h"

double get_double(MemoryJS & m, std::string const& key) {
    auto a = m.get(key);
    auto res_str = std::any_cast<std::string>(a);
    Document d;
    d.Parse(res_str.c_str());
    return d.GetDouble();
}

double get_double_any(std::any a) {
    auto res_str = std::any_cast<std::string>(a);
    Document d;
    d.Parse(res_str.c_str());
    return d.GetDouble();
}

int main() {

    bool success = true;

    bt::MemoryJS m;
    m.add("a", VarScope::INNER, "0");
    m.add("b", VarScope::OUTPUT, R"({"a":{"a":1}})");
    m.set("a", "b.a.a + 1");

    success &= get_double(m,"a") == 2;

    REVEAL("Var property assign, arithmetic, add, set");

    sample ch = m.get_changes(VarScope::INNER);

    success &= (ch.size() == 1);

    success &= get_double_any(ch["a"]) == 2;

    REVEAL("Get inner changes");

    m.clear_changes(VarScope::INNER);

    ch = m.get_changes(VarScope::INNER);

    success &= ch.size() == 0;

    REVEAL("Clear changes");

    m.eval("a = a ** 3");

    success &= get_double(m,"a") == 8;

    REVEAL("Eval");

    m.set_expr("a", "(b.a.a + 7)**2");

    success &= get_double(m, "a") == 64;

    REVEAL("Set_expr");

    m.set_expr("b.a.a", "15");
    m.set_expr("a", "b.a.a");
    success &= get_double(m, "a") == 15;

    REVEAL("Set_expr complex")

    ch = m.get_changes(VarScope::OUTPUT);

    success &= ch.size() == 1;
    success &= get_double(m, "a") == 15;
    std::string b_str = std::any_cast<std::string>(ch["b"]);
//    std::cout << b_str << std::endl;
    success &=  b_str == R"({"a":{"a":15}})";

    REVEAL("Get output changes & complex output")

    m.add("z", VarScope::OUTPUT_NO_SEND_ZERO, "1");
    m.set("z","0");
    success &= m.get_changes(VarScope::OUTPUT_NO_SEND_ZERO).size() == 0;
    m.set("z", "3");
    success &= m.get_changes(VarScope::OUTPUT_NO_SEND_ZERO).size() == 1;

    REVEAL("Output No Zero scope");

    m.add("zz", VarScope::OUTPUT, "1");
    success &= m.eval_bool("zz == 1") && m.eval_bool("zz + 3 != 5");
    REVEAL("Eval bool ")

    success &= m.has_var("zz");

    REVEAL("Has var (positive test)")

    success &= !m.has_var("zy");

    REVEAL("Has var (negative test)")

    success &= m.test_expr("z += 2");

    REVEAL("Test expr (positive test)")

    success &= !m.test_expr("ascxvbcdf zsfdEREF +=EW343");

    REVEAL("Test expr (negative test)")

    m.add("zzz", VarScope::INNER, double(2.0));
    success &= m.test_expr("zzz = 5");

    REVEAL("Test expr (positive, one more)")

    success &= m.get_double("zzz") == 2;

    REVEAL("Test expr restore changes")

}