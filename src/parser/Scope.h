//
// Created by safoex on 26.07.19.
//

#ifndef ABTM_SCOPE_H
#define ABTM_SCOPE_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "BaseParser.h"
#include "NodesParser.h"

namespace bt {
    class Scope : public ParserWithModules {
    public:
        Scope(Builder& builder, std::vector<std::pair<std::vector<std::string>, BaseParser*>> const& parsers)
            : ParserWithModules(builder, parsers) {};

        void parse(std::string const& id, YAML::Node const &yaml_node) override {
            try {
                for (auto const &yn: yaml_node)
                    parsers[yn.first.as<std::string>()]->parse(yn.first.as<std::string>(), yn.second);
            }
            catch (std::exception &e) {
                throw std::runtime_error("Error in scope " + id + ": " + e.what());
            }
        }

    };
}

#endif //ABTM_SCOPE_H
