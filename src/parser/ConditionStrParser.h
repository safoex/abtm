//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_CONDITIONSTRPARSER_H
#define ABTM_CONDITIONSTRPARSER_H


#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "LeafStrParser.h"
namespace bt {
    class ConditionStrParser : public LeafStrParser {
    public:
        explicit ConditionStrParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ConditionStrParser() override = 0;
    };
}

#endif //ABTM_CONDITIONSTRPARSER_H
