//
// Created by safoex on 09.07.19.
//

#include "Builder.h"
#include <queue>

namespace bt {
    Builder::Builder(bt::Tree *tree) : tree(tree) {}

    void Builder::make_graph() {
        std::queue<std::string> nodes;
        nodes.push(tree->get_root_name());
        while(!nodes.empty()) {
            std::string parent = nodes.front();
            nodes.pop();
            for(auto const& child: graph[parent]) {
                switch (store[child]->node_class() ) {
                    case Node::Action: tree->add_node(parent, dynamic_cast<Action *>(store[child])); break;
                    case Node::Condition: tree->add_node(parent, dynamic_cast<Condition*>(store[child])); break;
                    default: tree->add_node(parent, dynamic_cast<Sequential*>(store[child])); break;
                }
                nodes.push(child);
            }
        }
    }

}