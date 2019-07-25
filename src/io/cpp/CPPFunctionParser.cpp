//
// Created by safoex on 25.07.19.
//

#include "CPPFunctionParser.h"

namespace bt{
    CPPFunctionParser::CPPFunctionParser(bt::Builder &builder) : BaseParser(builder) {}

    void CPPFunctionParser::insert(std::string const &name, const bt::ExternalFunction &function) {
        functions[name] = function;
    }

    std::string CPPFunctionParser::get_var_name(std::string const &function_name, std::string const &var_type) const {
        return "__function_" + function_name + "_" + var_type;
    }

    void CPPFunctionParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        if(!functions.count(id))
            throw std::runtime_error("Error! No function \"" + id + "\"  known");

        auto f = functions[id];

        auto const& yn = yaml_node;

        // ------ load flags  ----------------------------------------------------------

        std::set<std::string> flags;

        if(yn["flags"]) {
            try {
                for(auto const& f: yn["flags"])
                    flags.insert(f.as<std::string>());
            }
            catch (std::exception &e) {
                throw std::runtime_error("Error while loading flags of " + id + " function");
            }
        }

        // ------ with/out try-catch block ---------------------------------------------


        if(flags.count("try") || flags.count("try-catch")) {
            f = [f, id, this](sample const& s) -> sample {
                sample r;
                try {
                    r = f(s);
                }
                catch (std::exception &e) {
                    r = {{get_var_name(id, "error"), 1}};
                }
                return r;
            };
        }

        // ------ add required vars ----------------------------------------------------

        sample vars; // so called required vars

        if(yn["vars"]) {
            auto const& vn = yn["vars"];
            if(vn.IsSequence()) {

                try {
                    for (auto const &v: vn) {
                        vars.insert({v.as<std::string>(),0});
                    }
                }
                catch(std::exception& e) {
                    throw std::runtime_error("Error while loading vars of " + id + " function: " + e.what());
                }
            }
            else if(vn.IsScalar()) {

            }
            else throw std::runtime_error("Error: unsupported definition of vars in " + id + " function");
        }

        // ------ add variables for call and return ------------------------------------


        f = [f, id, this](sample const& s) -> sample {
            sample r = f(s);
            r.insert({{this->get_var_name(id, "return"), 1}});
        };

        auto const& required_vars = vars;

        sample trigger_vars;
        trigger_vars.insert({get_var_name(id, "call"),0});;

        if(flags.count("on_each_var"))
            trigger_vars.insert(vars.begin(), vars.end());

        // ------ TODO: various threading policies are supported -----------------------------

        IOBase* io_function;

        if(flags.count("async")) {

        }
        else {
            io_function = new SynchronousAction(f, required_vars, trigger_vars);
        }

        io_modules.push_back(io_function);
    }


    CPPFunctionParser::~CPPFunctionParser() {
        for(auto io_ptr: io_modules)
            delete io_ptr;
    }

}