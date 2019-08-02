//
// Created by safoex on 02.08.19.
//

#ifndef ABTM_MEMORYJS_H
#define ABTM_MEMORYJS_H

#include <defs.h>
#include <duktape.h>
#include "MemoryBase.h"
#include <fstream>
#include <sstream>

namespace bt {
    class MemoryJS : public MemoryBase {
        duk_context* ctx;
    private:
        void create_proxy() {
            duk_eval_string(ctx, R"()");
        }
        std::string get_scope_name(VarScope scope) {
            switch(scope) {
                case VarScope ::INNER: return "input"; break;
                case VarScope ::OUTPUT: return "output"; break;
                case VarScope ::OUTPUT_NO_SEND_ZERO: return "output_no_zero"; break;
                case VarScope ::PURE_OUTPUT: return "pure_output"; break;
                default: return "input";
            }
        }

        void eval_with_exception_noresult(std::string eval_str, std::string error_str) {
            duk_push_string(ctx, eval_str.c_str());
            if(duk_peval_noresult(ctx) != 0) {
                throw std::runtime_error(error_str);
            }
        }

        void eval_with_exception(std::string eval_str, std::string error_str) {
            duk_push_string(ctx, eval_str.c_str());
            if(duk_peval(ctx) != 0) {
                throw std::runtime_error(error_str);
            }
        }

        std::string get_from_any(std::any const& a, std::string const& key) {
            std::string s;
            try {
                s = std::any_cast<std::string>(a);
            }
            catch(std::bad_any_cast &e) {
                throw std::runtime_error("Error: argument \"init\" for add with key: " + key + " is not a std::string!");
            }
            return s;
        }

        void restore_changes(VarScope scope) {

        }

        void restore_changes() {

        }
    public:
        MemoryJS(std::string file = "memory.js") {
            ctx = duk_create_heap_default();
            std::ifstream memory_js_lib(file);
            std::stringstream buffer;
            buffer << memory_js_lib.rdbuf();

            eval_with_exception_noresult(buffer.str(), "Error: bad javascript memory library provided!")

        }

        void add(std::string const& key, VarScope scope, std::any const& init) override {
            std::stringstream cmd;

            std::string init_str = get_from_any(init, key);

            cmd << "add(" << get_scope_name(scope) << ", " << R"(Duktape.enc('jc',")"
                    << init_str << R"("))"   << ", " << key << ");";

            eval_with_exception_noresult(cmd.str(),
             "Error: wrong parameters for add function! key: " + key + ", scope: "
             + get_scope_name(scope) + ", init: " + init_str
            );
        }

        void add(std::string const& key, VarScope scope) override {
            add(key, scope, std::any(std::string("0")));
        }

        void eval(std::string const& expr) {
            eval_with_exception(expr, "Error: bad expression provided: \"" + expr + "\"!");
        }

        void set(std::string const& key, std::any const& v) {
            std::string json_v = get_from_any(v, key);

            eval(key + " = Duktape.dec("+ json_v +")");
        }

        std::any get(std::string const& key)  {
            eval("Duktape.enc(\'jc\', window[" + key + "])");
            return std::string(duk_get_string(ctx, -1));
        }

        sample get_changes() {

        }

        ~MemoryJS() {
            duk_destroy_heap(ctx);
        }
    };
}

#endif //ABTM_MEMORYJS_H
