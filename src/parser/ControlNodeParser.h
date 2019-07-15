//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_CONTROLNODEPARSER_H
#define ABTM_CONTROLNODEPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "NodeParser.h"

namespace bt {
    class ControlNodeParser : public NodeParser {
    public:
        explicit ControlNodeParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~ControlNodeParser() override = default;

    protected:
        std::vector<std::string> parse_children(std::string const& id, YAML::Node const& node);
    };
}

#endif //ABTM_CONTROLNODEPARSER_H
