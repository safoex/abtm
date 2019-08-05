//
// Created by safoex on 03.08.19.
//

#ifndef ABTM_CONDITIONJSPARSER_H
#define ABTM_CONDITIONJSPARSER_H

#include "LeafJSParser.h"

namespace bt {
    class ConditionJSParser : public LeafJSParser {
    public:
        explicit ConditionJSParser(Builder& builder, VariablesInNodeParser* vparser = nullptr);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ConditionJSParser() override = default;
    };
}


#endif //ABTM_CONDITIONJSPARSER_H
