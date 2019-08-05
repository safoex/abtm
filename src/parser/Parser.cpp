//
// Created by safoex on 09.07.19.
//

#include "Parser.h"

namespace bt {
    Parser::Parser(Builder& builder) : ParserWithModules(builder) {}

    std::vector<std::string> Parser::sort_parsers() {
        dictOf<int> np; //numbered parsers
        std::vector<std::string> rnp(parsers.size(), ""); // reverse np
        int n = 0;

        // init np (numbered parsers) and rnp
        for(auto const& p: parsers)
            rnp[n] = p.first,
            np[p.first] = n++;


        std::vector<std::vector<int>> g(n, std::vector<int>());
        std::vector<char> used(n,0);
        std::vector<int> ans;

        // init graph and check for parsers that were not loaded
        for(auto const& p: parsers) {
            for(auto const& r: p.second->requirements) {
                if(parsers.count(r))
                    g[np[p.first]].push_back(np[r]);
                else
                    throw std::runtime_error("parser \'" + r + "\' required for parser \'" + p.first + "\'");
            }
            for(auto const& r: p.second->optional_requirements) {
                if(parsers.count(r))
                    g[np[p.first]].push_back(np[r]);
            }
        }

        std::function<void(int)> dfs = [&](int v) {
            used[v] = 1;
            for(size_t i=0; i<g[v].size(); ++i) {
                int to = g[v][i];
                if (used[to] == 0)
                    dfs (to);
                else if(used[to] == 1)
                    throw std::runtime_error("parser \'" + rnp[to] + "\' is in cyclic dependence");
            }
            used[v] = 2;
            ans.push_back (v);
        };

        std::function<void()> topological_sort = [&]() {
            for (int i=0; i<n; ++i)
                used[i] = false;
            ans.clear();
            for (int i=0; i<n; ++i)
                if (used[i] == 0)
                    dfs (i);
//            reverse (ans.begin(), ans.end());
        };

        // sort parsers
        topological_sort();

        std::vector<std::string> ans_string;
        ans_string.reserve(ans.size());

        for(auto const& p_int: ans)
            ans_string.push_back(rnp[p_int]);

        return ans_string;
    }

    void Parser::loadYamlFile(std::string const &filename) {
        YAML::Node config;
        try {
            config = YAML::LoadFile(filename);
        }
        catch(YAML::Exception &e) {
            throw YAML::Exception(YAML::Mark::null_mark(), "smth wrong with tree description file " + (std::string)e.what());
        }

        for(auto const& p: sort_parsers()) {
            if(config[p])
                try {
                    std::cout << "Parsing " << p << std::endl;
                    parsers[p]->parse(p, config[p]);
                }
                catch (std::exception& e) {
                    throw std::runtime_error(std::string("Error loading ") + p + ": " + e.what());
                }
        }

    }

    void Parser::parse(std::string const &id, YAML::Node const &yaml_node) {
        parsers[id]->parse(id, yaml_node);
        // TEMPORARY HACK
    }

    Parser::Parser(bt::Builder &builder, std::vector<std::pair<std::vector<std::string>, bt::BaseParser *>> const& parsers)
        : ParserWithModules(builder, parsers) {}
}