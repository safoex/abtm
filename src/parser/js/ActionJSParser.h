//
// Created by safoex on 03.08.19.
//

#ifndef ABTM_ACTIONJSPARSER_H
#define ABTM_ACTIONJSPARSER_H

#include "LeafJSParser.h"

namespace bt {
    class ActionJSParser : public LeafJSParser {
    public:
        explicit ActionJSParser(Builder& builder, VariablesInNodeParser* vparser = nullptr);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ActionJSParser() override = default;

    };
}


#endif //ABTM_ACTIONJSPARSER_H
