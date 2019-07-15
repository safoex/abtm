//
// Created by safoex on 15.07.19.
//

#include "CommonParser.h"

namespace bt{
    CommonParser::CommonParser(bt::Builder &builder): BaseParser(builder) {}

    void CommonParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        auto const& name = builder.tree->get_root_name();
        auto const& info = yaml_node;
        std::string root_child;
        load<std::string>(info, "root_child", root_child);
        if(root_child.empty()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "no child of root wrote!");
        }

        builder.graph[name] = {root_child};
    }
}