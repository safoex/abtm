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
#include <queue>
#include <Node.h>
#include <string>

namespace bt {

    /* ------------------- Template declaration --------------------------------------------------------- */

    template<class _Tree>
    class BuilderBase {
    public:
        dictOf<Node*> store; // {node name e.g. "set_mode", {node type e.g. "action", node ptr [...]}}
        dictOf<std::vector<std::string>> graph; // {parent node, list of children};
        dictOf<dictOf<std::any>> view_graph; // {node name, {parameters}}
        dictOf<std::any> extra;
        _Tree* tree;
        explicit BuilderBase(_Tree* tree);
        void make_graph();
        virtual ~BuilderBase() = default;
    };


    /* ------------------- Template implementation ------------------------------------------------------ */

    template<class _Tree>
    BuilderBase<_Tree>::BuilderBase(_Tree *tree) : tree(tree) {}

    template<class _Tree>
    void BuilderBase<_Tree>::make_graph() {
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

    typedef BuilderBase<Tree> Builder;
}


#endif //ABTM_BUILDER_H
