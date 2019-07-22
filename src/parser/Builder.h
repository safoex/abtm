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
#include <stack>

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
        enum TREE_DESCRIPTION {
            DOT,
            DOT_STATES
        };
        explicit BuilderBase(_Tree* tree);
        void make_graph();
        std::string get_dot_description(TREE_DESCRIPTION td);
        virtual ~BuilderBase();
    };


    /* ------------------- Template implementation ------------------------------------------------------ */

    template<class _Tree>
    BuilderBase<_Tree>::BuilderBase(_Tree *tree) : tree(tree) {
        extra["dot_colors"] = dictOf<std::pair<std::string, std::string>> {
                {"sequence",{"white","->"}},
                {"selector",{"white","?"}},
                {"parallel",{"white","="}},
                {"skipper", {"white","=>"}},
                {"action",{"green",""}},
                {"condition",{"orange",""}},
                {"default", {"\"#9262d1\"", "<...>"}}
        };
    }

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

    template <class _Tree>
    std::string BuilderBase<_Tree>::get_dot_description(TREE_DESCRIPTION td) {
        if(td == DOT || td == DOT_STATES) {
            std::string result;
            result += "digraph g {\n";
            result += "node [shape=rectangle, style=filled, color=white];\n";


            dictOf<std::pair<std::string, std::string>> gv_names;

            for (auto const &p:store) {
                auto const &n = *p.second;
                if(!view_graph.count(n.id()))
                    continue;
                std::string desc;
                if(td == DOT_STATES)
                    desc = STATE(n.state());
                else {
                    if(view_graph[n.id()].count("class"))
                        desc = std::any_cast<std::string>(view_graph[n.id()]["class"]);
                    else
                        desc = n.classifier();
                }

                //define node

                std::string gv_node_name;
                gv_node_name += "\"";
                auto dot_colors = std::any_cast<dictOf<std::pair<std::string, std::string>>>(extra["dot_colors"]);
                std::string type = std::any_cast<std::string>(view_graph[n.id()]["type"]);
                std::pair<std::string, std::string> p2;
                if(dot_colors.count(type))
                    p2 = dot_colors.at(type);
                else
                    p2 = dot_colors.at("default");
                std::string color = p2.first, hat = p2.second;
                if(view_graph[n.id()].count("color"))
                    color = std::any_cast<std::string>(view_graph[n.id()]["color"]);
                if(n.hide_further) color = "\"#16abc9\"";
                if(!hat.empty())
                    gv_node_name += hat + "\n";
                gv_node_name += "" + n.id() + "";
                if(!desc.empty())
                    gv_node_name += "\n" + desc;
                gv_node_name += "\"";
                std::string fontcolor = "black";
                if(n.node_class() != Node::Action && n.node_class() != Node::Condition && desc == "RUNNING")
                    fontcolor = "red";
                gv_names[n.id()] = {"\"" + n.id() + "\"", "\""+n.id()+"\"" + "[label=" +  gv_node_name + ", color=" + color
                                                            + ", fontcolor=" + fontcolor + "];\n"};

            }

            std::stack<std::string> _dfs;
            _dfs.push(graph[tree->get_root_name()][0]);
            std::string constraints;
            int n_in_dfs = 0;
            while (!_dfs.empty()) {
                n_in_dfs++;
                auto s = _dfs.top();
                auto& _n = view_graph[s];
                _dfs.pop();
                result += gv_names[s].second;

                std::string constraint = "{ rank = same;\n";
                bool hide = _n.count("hide") && ((int&)_n["hide"]);
                if(_n.count("children") && !hide ) {
                    auto const& children = std::any_cast<std::vector<std::string>>(_n["children"]);
                    for(auto cit = children.rbegin(); cit != children.rend(); cit++) {
                        _dfs.push(*cit);
                    }
                    for(auto cit = children.begin(); cit != children.end(); cit++) {
                        result += gv_names[s].first + " -> " + gv_names[(*cit)].first + ";\n";
                        if (cit != children.begin()) {
                            constraint += " -> ";
                        }
                        constraint += gv_names[(*cit)].first;
                    }
                    constraint += "[style=invis];\n}\n";
                    if (children.size() > 1) constraints += constraint;
                }


            }


            result += constraints;
            result += "\n}";

            std::cout << "Nodes in dfs: " << n_in_dfs << std::endl;
            return result;
        }
        else return "";
    }

    template <class _Tree>
    BuilderBase<_Tree>::~BuilderBase() {

    }
    typedef BuilderBase<Tree> Builder;
}


#endif //ABTM_BUILDER_H
