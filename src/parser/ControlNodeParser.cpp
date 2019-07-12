//
// Created by safoex on 09.07.19.
//

#include "ControlNodeParser.h"
#include <any>

namespace bt {

    ControlNodeParser::ControlNodeParser(bt::Builder &builder) : NodeParser(builder){};

    std::vector<std::string> ControlNodeParser::parse_children(std::string const& id, YAML::Node const &node) {
        std::vector<std::string> children;
        try {
            auto const& children_list = node["children"];
            for(auto const& c: children_list) {
                children.push_back(c.as<std::string>());
            }
        }
        catch(YAML::Exception & e) {
            LOG_DEBUG("ar_bt_test : on parsing "+id + " " + e.what());
            throw YAML::Exception(YAML::Mark::null_mark(), e.what());
        }
        if(children.empty()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "Empty children list at " + id);
        }
        return children;
    }

    void ControlNodeParser::parse(std::string const& id, YAML::Node const &yaml_node) {
        NodeParser::parse(id, yaml_node);


        // get hide parameter for visualization (if true, then hide children)
        builder.view_graph[id]["hide"] = load<int>(yaml_node, "hide");

        if(type == "sequence") add_to_builder(new Sequence(id, builder.tree->get_memory(), classifier));
        if(type == "selector") add_to_builder(new Selector(id, builder.tree->get_memory(), classifier));
        if(type == "skipper") add_to_builder(new RunningSkippingSequence(id, builder.tree->get_memory(), classifier));
        if(type == "paralel") add_to_builder(new Parallel(id, builder.tree->get_memory(), classifier));

        // get children; returns exception if there are no children provided
        auto children(parse_children(id, yaml_node));
        builder.graph[id] = children;
        builder.view_graph[id]["children"] = children;


    }
};