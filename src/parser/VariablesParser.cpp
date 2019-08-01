//
// Created by safoex on 15.07.19.
//

#include "VariablesParser.h"

namespace bt {
    void VariablesParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        if(id == "variables")
            return add_variables(yaml_node);
//        else if(id == "set")
//            return set_variables(yaml_node);
    }

    void VariablesParser::add_variables(YAML::Node const &yaml_node) {
        auto const& variables = yaml_node;
        Tree::dict<Memory<double>::Scope> scopes {
                {"input",Memory<double>::INNER},
                {"output",Memory<double>::OUTPUT},
                {"pure_output",Memory<double>::PURE_OUTPUT},
                {"output_no_send_zero",Memory<double>::OUTPUT_NO_SEND_ZERO}
        };

        for(auto const& p: variables) {
            auto const& sc_str = p.first.as<std::string>();
            if(!scopes.count(sc_str))
                throw YAML::Exception(YAML::Mark::null_mark(), "wrong scope name "+sc_str);
            auto scope = scopes[sc_str];

            try {
                for (auto const &v: p.second)
                    builder.tree->get_memory().add_variable(v.as<std::string>(), scope);
            }
            catch(YAML::Exception & e) {
                throw YAML::Exception(YAML::Mark::null_mark(), "error while loading variables at scope " + sc_str);
            }
        }
    }

    void VariablesParser::set_variables(YAML::Node const &yaml_node) {
        auto const& vars = yaml_node;
        for(auto const& p: vars) {
            try {
                std::string const& v_name = p.first.as<std::string>();
                double v_value = p.second.as<double>();
                builder.tree->get_memory().set(v_name, v_value);
            }
            catch(YAML::Exception & e) {
                throw YAML::Exception(YAML::Mark::null_mark(), "error while initializing variables " + (std::string)e.what());
            }
        }
    }
}