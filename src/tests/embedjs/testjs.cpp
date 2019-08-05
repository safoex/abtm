//
// Created by safoex on 01.08.19.
//

#include <duktape.h>
#include <duk_config.h>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include "defs.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


int main(int argc, char *argv[]) {
    duk_context *ctx = duk_create_heap_default();
    std::ifstream mem("../src/tests/embedjs/memory.js");
    std::string content( (std::istreambuf_iterator<char>(mem) ),
                         (std::istreambuf_iterator<char>()    ) );
    duk_eval_string(ctx, content.c_str());
    std::string a,b;
    try {
        duk_eval_string(ctx, R"(add("input",0,"a"))");
        duk_eval_string(ctx, R"(a = 2;)");
        duk_eval_string(ctx, R"(poll_changes();get_changes("input"))");
        a = duk_get_string(ctx, -1);
        duk_eval_string(ctx, R"(restore_changes();)");
        duk_eval_string(ctx, R"(add("input",0,"b"))");
        duk_eval_string(ctx, R"(b = a)");
        duk_eval_string(ctx, R"(poll_changes();get_changes("input"))");
        duk_eval_string(ctx, R"(apply_changes();clear_changes();)");
//        duk_eval_string(ctx, "x = ;");
        std::cout << a << std::endl;
        std::string comp = ("a = Duktape.dec('jc', '" + a + "')");
        duk_eval_string(ctx, comp.c_str());
        duk_eval_string(ctx, R"(poll_changes();get_changes("input"))");
        duk_eval_string(ctx, R"(a.a < 3 && b == 1)");
        std::cout <<"AAA " << duk_get_boolean(ctx, -1) << std::endl;
        b = duk_get_string(ctx, -2);

    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << a << std::endl;

    rapidjson::Document d;
    d.Parse(a.c_str());
    bt::sample result;
    for(auto k = d.MemberBegin(); k != d.MemberEnd(); ++k) {
        std::string const& var = k->name.GetString(), val =  k->value.GetString();
        result[var] = val;
    }

    rapidjson::Document d2;
    d2.Parse(std::any_cast<std::string>(result["a"]).c_str());
//    d2.Parse("asd");
    std::cout << d2.GetDouble() << std::endl;
    std::cout << b << std::endl;
    std::optional<double> z, y(2);
    std::cout << (z ? "YES" : "NO")  << '\t' << y.value() << std::endl;


    duk_destroy_heap(ctx);
    return 0;
}