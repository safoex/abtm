//
// Created by safoex on 09.07.19.
//

#include "LeafStrParser.h"

namespace bt {
    LeafStrParser::LeafStrParser(bt::Builder &builder) : NodeParser(builder), expr(builder.tree->get_memory()) {}

    inline void LeafStrParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        NodeParser::parse(id, yaml_node);
    }
}