//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_NODEPARSER_H
#define ABTM_NODEPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "BaseParser.h"

namespace bt {
    class NodeParser : public BaseParser {
    public:
        explicit NodeParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~NodeParser() override = default;

    protected:
        std::string type, classifier;
        void add_to_builder(std::string const& id, Node* built_node);
    };
};

#endif //ABTM_NODEPARSER_H
