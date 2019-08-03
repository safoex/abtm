//
// Created by safoex on 03.08.19.
//

#ifndef ABTM_ACTIONJSPARSER_H
#define ABTM_ACTIONJSPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "parser/Builder.h"
#include "parser/Parser.h"
#include "parser/NodeParser.h"

namespace bt {
    class ActionJSParser : public NodeParser {
    public:
        explicit ActionJSParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ActionJSParser() override = default;

    };
}


#endif //ABTM_ACTIONJSPARSER_H
