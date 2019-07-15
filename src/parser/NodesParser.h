//
// Created by safoex on 15.07.19.
//

#ifndef ABTM_NODESPARSER_H
#define ABTM_NODESPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "ParserWithModules.h"

namespace bt {
    class NodesParser : public ParserWithModules {
    public:
        NodesParser(Builder& builder);

        NodesParser(Builder& builder, std::vector<std::pair<std::vector<std::string>, BaseParser*>> const& parsers);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~NodesParser() override = default;

    };
}

#endif //ABTM_NODESPARSER_H
