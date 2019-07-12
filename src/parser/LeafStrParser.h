//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_LEAFSTRPARSER_H
#define ABTM_LEAFSTRPARSER_H


#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "NodeParser.h"
#include "ExpressionParser.h"


namespace bt {
    class LeafStrParser : public NodeParser {
    public:
        explicit LeafStrParser(Builder& builder);

        inline void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~LeafStrParser() override = 0;

    protected:
        ExpressionParser expr;
    };
};

#endif //ABTM_LEAFSTRPARSER_H
