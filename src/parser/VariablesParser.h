//
// Created by safoex on 15.07.19.
//

#ifndef ABTM_VARIABLESPARSER_H
#define ABTM_VARIABLESPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "BaseParser.h"


namespace bt {
    class VariablesParser : public BaseParser {
    protected:
        void add_variables(YAML::Node const& yaml_node);
        void set_variables(YAML::Node const& yaml_node);
    public:
        VariablesParser(Builder& builder) : BaseParser(builder) {}

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~VariablesParser() = default;
    };

    class SetVariablesParser : public BaseParser {
    protected:
        void set_variables(YAML::Node const& yaml_node) {
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

    public:
        SetVariablesParser(Builder& builder) : BaseParser(builder) {
            requirements.push_back("variables");
        }

        void parse(std::string const& id, YAML::Node const &yaml_node) override {
            return set_variables(yaml_node);
        };

        ~SetVariablesParser() = default;
    };

}

#endif //ABTM_VARIABLESPARSER_H
