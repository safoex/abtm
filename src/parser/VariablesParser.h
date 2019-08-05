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
    public:
        explicit VariablesParser(Builder& builder) : BaseParser(builder) {}

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~VariablesParser() override = default;
    };

    class SetVariablesParser : public BaseParser {
    protected:
        void set_variables(YAML::Node const& yaml_node) {
            auto const& vars = yaml_node;
            for(auto const& p: vars) {
                try {
                    std::string const& v = p.first.as<std::string>();
                    std::string value;
                    if(p.second.IsScalar()) {
                        value = p.second.as<std::string>();
                    }
                    else throw std::runtime_error("Initialization value for var " + v +
                                                  "should be either double or string");
                    std::cout << v << ' ' << value << std::endl;
                    builder.tree->get_memory().set_expr(v, value);
                }
                catch(YAML::Exception & e) {
                    throw YAML::Exception(YAML::Mark::null_mark(), "error while initializing variables " + (std::string)e.what());
                }
                for(auto sc: {VarScope::INNER, VarScope::OUTPUT}) {
                    builder.tree->get_memory().get_changes(sc);
                    builder.tree->get_memory().clear_changes(sc);
                }
            }
        }

    public:
        explicit SetVariablesParser(Builder& builder) : BaseParser(builder) {
            requirements.push_back("variables");
        }

        void parse(std::string const& id, YAML::Node const &yaml_node) override {
            return set_variables(yaml_node);
        };

        ~SetVariablesParser() override = default;
    };

    class VariablesInNodeParser : public BaseParser {
    protected:
        void add_and_set_variables(YAML::Node const& yaml_node) {
            auto const& variables = yaml_node;
            Tree::dict<VarScope > scopes {
                    {"input",VarScope::INNER},
                    {"output",VarScope::OUTPUT},
                    {"pure_output",VarScope::PURE_OUTPUT},
                    {"output_no_send_zero",VarScope::OUTPUT_NO_SEND_ZERO}
            };
            std::cout << yaml_node << std::endl;
            for(auto const& p: variables) {
                auto const& v = p.first.as<std::string>();
                std::string scope, value;
                double v_d = 0;
                std::cout << '\t' << "adding var " << v << std::endl;
                if(p.second.IsSequence()) {
                    int i = 0;
                    for(auto const& p2: p.second) {
                        if(i == 0) {
                            if(p2.IsScalar()) {
                                scope = p2.as<std::string>();
                            }
                            else throw std::runtime_error("Scope in \"add\" block should be a string!");
                        }
                        else if(i == 1) {
                            if(p2.IsScalar()) {
                                try {
                                    v_d = p2.as<double>();
                                }
                                catch(YAML::RepresentationException &e) {
                                    value = p2.as<std::string>();
                                }
                            }
                            else throw std::runtime_error("Initialization value for var " + v +
                            "should be either double or string");
                        }
                        else throw std::runtime_error("Useless values (>2) given in \"add\" block!");
                        i++;
                    }
                }
                else {
                    if(p.second.IsScalar()) {
                        scope = p.second.as<std::string>();
                    }
                    else throw std::runtime_error("Scope should be string in \"add\" block!");
                }

                if(!scopes.count(scope))
                    throw YAML::Exception(YAML::Mark::null_mark(), "wrong scope name "+scope);
                auto sc = scopes[scope];

                try {
                    if(value.empty())
                        builder.tree->get_memory().add(v, sc, v_d);
                    else
                        builder.tree->get_memory().add(v, sc, value);

                }
                catch(YAML::Exception & e) {
                    throw YAML::Exception(YAML::Mark::null_mark(), "error while loading variables at scope " + scope);
                }
            }
        }
    public:
        explicit VariablesInNodeParser(Builder& builder) : BaseParser(builder) {}

        void parse(std::string const& id, YAML::Node const &yaml_node) override {
            add_and_set_variables(yaml_node);
        }

        ~VariablesInNodeParser() override = default;
    };


}

#endif //ABTM_VARIABLESPARSER_H
