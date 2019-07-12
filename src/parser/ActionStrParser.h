//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_ACTIONSTRPARSER_H
#define ABTM_ACTIONSTRPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "LeafStrParser.h"

namespace bt {
    class ActionStrParser : public LeafStrParser {
    public:
        explicit ActionStrParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ActionStrParser() override = 0;

    };
}

#endif //ABTM_ACTIONSTRPARSER_H
