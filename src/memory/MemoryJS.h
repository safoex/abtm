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
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_set>
#include <iomanip>

namespace bt {
    class MemoryJS : public MemoryBase {
        duk_context* ctx;
        std::unordered_set<std::string> known_vars;
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

        void eval_with_exception_noresult(std::string const& eval_str, std::string const& error_str) {
            duk_push_string(ctx, eval_str.c_str());
            if(duk_peval_noresult(ctx) != 0) {
                throw std::runtime_error(error_str);
            }
        }

        void eval_with_exception(std::string const& eval_str, std::string const& error_str) {
            duk_push_string(ctx, eval_str.c_str());
            if(duk_peval(ctx) != 0) {
                throw std::runtime_error(error_str);
            }
        }

        static std::string get_from_any(std::any const& a, std::string const& key) {
            std::string s;
            try {
                s = std::any_cast<std::string>(a);
            }
            catch(std::bad_any_cast &e) {
                try {
                    s = std::any_cast<const char*>(a);
                }
                catch(std::bad_any_cast &e) {
                    try{
                        std::stringstream ss;
                        ss << std::fixed;
                        ss << std::setprecision(9);
                        ss << std::any_cast<double>(a);
                        s = ss.str();
                    }
                    catch (std::bad_any_cast &e) {
                        throw std::runtime_error(
                            "Error: argument \"init\" for add with key: " + key + " is not a std::string!");
                    }
                }
            }
            return s;
        }

        inline void __simple_call(std::string const& func) {
            eval_with_exception(func, "Error: something wrong with " +func + "!");
        }

        inline void restore_changes(VarScope scope) {
            __simple_call("restore_changes(\"" + get_scope_name(scope) + "\");");
        }

        inline void restore_changes() {
            __simple_call("restore_changes();");
        }

        void clear_changes(std::string const& scope) {
            __simple_call("apply_changes(\"" + scope+ "\");clear_changes(\"" + scope + "\")");
        }

        sample get_changes(std::string const& scope) {
            __simple_call("poll_changes(\"" + scope + "\");");
            __simple_call("get_changes(\"" + scope + "\");");
            std::string json_changes = duk_get_string(ctx, -1);
            rapidjson::Document d;
            d.Parse(json_changes.c_str());
            sample result;
            for(auto k = d.MemberBegin(); k != d.MemberEnd(); ++k) {
                std::string var = k->name.GetString(), val =  k->value.GetString();
                result[var] = val;
            }
            return result;
        }

    public:
        explicit MemoryJS(std::string const& file = "../src/tests/embedjs/memory.js") : MemoryBase() {
            ctx = duk_create_heap_default();
            std::ifstream memory_js_lib(file);
            std::string content( (std::istreambuf_iterator<char>(memory_js_lib) ),
                                 (std::istreambuf_iterator<char>()    ) );

            duk_eval_string(ctx, content.c_str());
            eval_with_exception_noresult(content, "Error: bad javascript memory library provided!");

        }

        void add(std::string const& key, VarScope scope, std::any const& init) override {
            std::stringstream cmd;

            if(has_var(key)) return;

            std::string init_str = get_from_any(init, key);

            cmd << "add('" << get_scope_name(scope) << "', " << init_str  << ", '" << key << "');";
            std::string const& cmd_str = cmd.str();
            eval_with_exception_noresult(cmd_str,
             "Error: wrong parameters for add function! key: " + key + ", scope: "
             + get_scope_name(scope) + ", init: " + init_str
            );

            known_vars.insert(key);
        }

        std::unordered_set<std::string> const& get_known_vars() override {
            return known_vars;
        }

        void add(std::string const& key, VarScope scope) override {
            add(key, scope, std::any(std::string("0")));
        }

        void eval(std::string const& expr) override {
            eval_with_exception(expr, "Error: bad expression provided: \"" + expr + "\"!");
        }

        void set(std::string const& key, std::any const& v) override {
            std::string json_v = get_from_any(v, key);
            std::cout <<"memset " << key + " = " + json_v + ";" << std::endl;
            eval(key + " = " + json_v + ";");
        }

        std::any get(std::string const& key) override {
            return get_string(key).value();
        }

        std::optional<double> get_double(std::string const& key) override {
            rapidjson::Document d;
            d.Parse(get_string(key).value().c_str());
            if(d.IsDouble()) return d.GetDouble();
            else if(d.IsInt64()) return d.GetInt64();
            else return {};
        }

        std::optional<bool> get_bool(std::string const& key) override {
            rapidjson::Document d;
            d.Parse(get_string(key).value().c_str());
            if(d.IsBool()) return d.GetBool();
            else return {};
        }

        std::optional<std::string> get_string(std::string const& key) override {
            eval("Duktape.enc('jc', window[\"" + key + "\"])");
            return std::string(duk_get_string(ctx, -1));
        }

        sample get_changes(VarScope scope) override {
            if(scope == OUTPUT) {
                sample result;
                for (auto sc: {OUTPUT, OUTPUT_NO_SEND_ZERO, PURE_OUTPUT}) {
                    auto r = get_changes(get_scope_name(sc));
                    result.insert(r.begin(), r.end());
                }
                return result;
            }
            else return get_changes(get_scope_name(scope));
        }

        void clear_changes(VarScope scope) override {
            if(scope == VarScope::OUTPUT) {
                clear_changes(get_scope_name(OUTPUT));
                clear_changes(get_scope_name(PURE_OUTPUT));
                clear_changes(get_scope_name(OUTPUT_NO_SEND_ZERO));
            }
            else clear_changes(get_scope_name(scope));
        }

        void set_expr(std::string const& key, std::string const& expr) override {
            eval(key + " = " + expr);
        }

        bool eval_bool(std::string const& expr) override {
            eval(expr);
//            std::cout << "eval of " << expr << " is " << duk_get_string(ctx, -1) << std::endl;
            return (bool)duk_get_boolean(ctx, -1);
        }

        bool has_var(std::string const& key) override {
            return eval_bool("\"" + key + "\" in window");
        }

        bool test_expr(std::string const& expr) override {
            duk_push_string(ctx, expr.c_str());
            bool test = duk_peval(ctx) == 0;
            __simple_call("poll_changes()");
            restore_changes();
            return test;
        }

        ~MemoryJS() override {
            duk_destroy_heap(ctx);
        }
    };
}

#endif //ABTM_MEMORYJS_H
