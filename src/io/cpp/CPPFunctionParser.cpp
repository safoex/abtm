//
// Created by safoex on 25.07.19.
//

#include "CPPFunctionParser.h"

namespace bt{
    CPPFunctionParser::CPPFunctionParser(bt::Builder &builder, MIMOCenter& mimo) : IOParser(builder, mimo) {}

    CPPFunctionParser::CPPFunctionParser(bt::Builder &builder, MIMOCenter& mimo,
            const std::vector<std::vector<bt::CPPFunction>> &cppFunctions) : IOParser(builder, mimo) {
        insert(cppFunctions);
    }

    CPPFunctionParser::CPPFunctionParser(bt::Builder &builder, MIMOCenter& mimo,
            const std::vector<bt::CPPFunction> &cppFunctions) : IOParser(builder, mimo) {
        insert(cppFunctions);
    }

    void CPPFunctionParser::insert(const std::vector<std::vector<bt::CPPFunction>> &cppFunctions) {
        for(auto const& vf: cppFunctions)
            insert(vf);
    }

    void CPPFunctionParser::insert(const std::vector<bt::CPPFunction> &cppFunctions) {
        for(auto const& f: cppFunctions)
            insert(f);
    }

    void CPPFunctionParser::insert(CPPFunction const& cppFunction) {
        functions.insert(make_pair(cppFunction.name, cppFunction));
    }

    std::string CPPFunctionParser::get_var_name(std::string const &function_name, std::string const &var_type) const {
        return "__function_" + function_name + "_" + var_type;
    }

    void CPPFunctionParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        for(auto const& fdefs: yaml_node)
            parse_one(fdefs.first.as<std::string>(), fdefs.second);
    }

    void CPPFunctionParser::parse_one(std::string const &id, YAML::Node const &yaml_node) {

        // ------ load function --------------------------------------------------------

        auto const& yn = yaml_node;

        if(!yn["name"])
            throw std::runtime_error("Error! No function name for instance " + id + " provided");
        else if(!functions.count(yn["name"].as<std::string>()))
            throw std::runtime_error("Error! No function \"" + id + "\"  known");

        ExternalFunction f = functions[yn["name"].as<std::string>()].f;

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

        // ------ add required vars and remap if needed ----------------------------------------------------

        sample vars = functions[id].required_vars; // so called required vars

        if(yn["remap"]) {
            auto const& vn = yn["remap"];
            if(vn.IsMap()) {
                try {
                    dictOf<std::string> remap;
                    for (auto const &v: vn) {
                        remap[v.first.as<std::string>()] = v.second.as<std::string>();
                    }
                    auto new_vars = vars;
                    for(auto const& v: remap) {
                        if(vars.count(v.first)) {
                            new_vars[v.second] = vars[v.first];
                            new_vars.erase(v.first);
                        }
                    }
                    vars = new_vars;
                    f = [f, this, remap] (sample const& s) -> sample {
                        sample remapped_sample = s;
                        for(auto const& v: remap) {
                            if(s.count(v.second)) {
                                remapped_sample[v.first] = s.at(v.second);
                                remapped_sample.erase(v.second);
                            }
                        }
                        return f(remapped_sample);
                    };
                }
                catch(std::exception& e) {
                    throw std::runtime_error("Error while loading remap of " + id + " function: " + e.what());
                }
            }
            else throw std::runtime_error("Error: unsupported definition of vars in " + id + " function");
        }

        // ------ add variables for call and return ------------------------------------


        f = [f, id, this](sample const& s) -> sample {
            sample r = f(s);
            r.insert({{this->get_var_name(id, "return"), double(1)}});
            return r;
        };

        auto const& required_vars = vars;

        sample trigger_vars;
        trigger_vars.insert({get_var_name(id, "call"), double(0)});;

        if(flags.count("on_each_var"))
            trigger_vars.insert(vars.begin(), vars.end());

        // ------ TODO: various threading policies are supported -----------------------------

        IOBase* io_function;

        if(flags.count("async")) {

        }
        else {
            io_function = new SynchronousAction(f, required_vars, trigger_vars);
        }

        register_module(io_function, functions[id].priority);
    }


}