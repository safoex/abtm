//
// Created by safoex on 09.07.19.
//

#include "NodeParser.h"

namespace bt {

    NodeParser::NodeParser(bt::Builder &builder) : BaseParser(builder){
        optional_requirements.push_back("variables");
    }

    void NodeParser::parse(std::string const &id, YAML::Node const &yaml_node) {

        if(!yaml_node.IsMap()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "Not a map! : " + id);
        }

        type = load<std::string>(yaml_node, "type");
        builder.view_graph[id]["type"] = std::any(type);
        classifier = load<std::string>(yaml_node, "class");
        builder.view_graph[id]["class"] = std::any(classifier);
    }

    void NodeParser::add_to_builder(std::string const& id, bt::Node *built_node) {
        builder.store[id] = built_node;
    }
};