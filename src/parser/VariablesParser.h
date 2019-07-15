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

}

#endif //ABTM_VARIABLESPARSER_H
