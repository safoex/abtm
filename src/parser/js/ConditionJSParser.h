//
// Created by safoex on 03.08.19.
//

#ifndef ABTM_CONDITIONJSPARSER_H
#define ABTM_CONDITIONJSPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "parser/Builder.h"
#include "parser/Parser.h"
#include "parser/NodeParser.h"

namespace bt {
    class ConditionStrParser : public NodeParser {
    public:
        explicit ConditionStrParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ConditionStrParser() override = default;
    };
}


#endif //ABTM_CONDITIONJSPARSER_H
