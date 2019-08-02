//
// Created by safoex on 01.08.19.
//

#include <duktape.h>
#include <duk_config.h>
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {
    duk_context *ctx = duk_create_heap_default();
    duk_eval_string(ctx, "var a = {a:1}, b = {b:2};");
    std::string a;
    try {
        duk_push_string(ctx, R"(Duktape.enc('jc', c))");
        duk_peval(ctx);
        //a = duk_get_string(ctx, -1);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    std::cout << a << std::endl;
    duk_destroy_heap(ctx);
    return 0;
}