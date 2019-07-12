//
// Created by safoex on 09.07.19.
//

#include "Parser.h"

namespace bt {
    Parser::Parser(Builder builder) : builder(builder) {}

    Parser::Parser(bt::Tree *tree) : builder(tree) {}

    void Parser::registerModule(std::string const &type, bt::BaseParser *parser) {
        parsers[type] = parser;
    }

    std::vector<std::string> Parser::sort_parsers() {
        dictOf<int> np; //numbered parsers
        int n = 0;
        for(auto const& p: parsers)
            np[p.first] = n++;
        std::vector<std::vector<int>> g(n, std::vector<int>());
        std::vector<bool> used(n,false);
        std::vector<int> ans;

        for(auto const& p: parsers) {
            for(auto const& r: p.second->requirements) {
                if(parsers.count(r))
                    g[np[p.first]].push_back(np[r]);
                else
                    throw std::runtime_error("parser \'" + r + "\' required for parser \'" + p.first + "\'");
            }
        }

        std::function<void(int)> dfs = [&](int v) {
            used[v] = true;
            for (size_t i=0; i<g[v].size(); ++i) {
                int to = g[v][i];
                if (!used[to])
                    dfs (to);
            }
            ans.push_back (v);
        };

        std::function<void()> topological_sort = [&]() {
            for (int i=0; i<n; ++i)
                used[i] = false;
            ans.clear();
            for (int i=0; i<n; ++i)
                if (!used[i])
                    dfs (i);
            reverse (ans.begin(), ans.end());
        };



    }

    void Parser::loadYamlFile(std::string const &filename) {
        YAML::Node config;
        try {
            config = YAML::LoadFile(filename);
        }
        catch(YAML::Exception &e) {
            throw YAML::Exception(YAML::Mark::null_mark(), "smth wrong with tree description file " + (std::string)e.what());
        }

        auto const& name = builder.tree->get_root_name();
        auto const& info = config["common"];
        std::string root_child;
        load<std::string>(info, "root_child", root_child);
        if(root_child == "") {
            throw YAML::Exception(YAML::Mark::null_mark(), "no child of root wrote!");
        }

        builder.graph[name] = {root_child};
    }
}