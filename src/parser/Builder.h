//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_BUILDER_H
#define ABTM_BUILDER_H

#include <Tree.h>
#include <defs.h>
#include <any>
#include <utility>
#include <unordered_map>
#include <vector>
#include <Node.h>
#include <string>

namespace bt {
    class Builder {
    public:
        dictOf<std::pair<std::string, Node*>> store; // {node name e.g. "set_mode", {node type e.g. "action", node ptr [...]}}
        dictOf<std::vector<std::string>> graph; // {parent node, list of children};
        dictOf<dictOf<std::any>> view_graph; // {node name, {parameters}}
        Tree* tree;
        Builder();
        virtual ~Builder();
    };
}


#endif //ABTM_BUILDER_H
