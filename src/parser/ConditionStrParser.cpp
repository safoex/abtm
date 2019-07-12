//
// Created by safoex on 09.07.19.
//

#include "ConditionStrParser.h"

namespace bt{
    ConditionStrParser::ConditionStrParser(bt::Builder &builder) : LeafStrParser(builder) {}

    void ConditionStrParser::parse(std::string const &id, YAML::Node const &node) {

        LeafStrParser::parse(id, node);

        std::set<std::string> sfr{"S","F","R"};
        Tree::dict<std::string> SFR{{"S",""},{"F",""},{"R",""}};

        std::unordered_set<std::string> used_vars;

        for(auto const& f: node) {
            if(sfr.count(f.first.as<std::string>()))
                SFR[f.first.as<std::string>()] = f.second.as<std::string>();
        }

        // used vars
        if(!node["used_vars"]) {
            for(auto const& f: SFR) {
                if(f.second != "default") {
                    auto uv = find_vars(f.second);
                    used_vars.insert(uv.begin(), uv.end());
                }
            }
        }
        else {
            if(node["used_vars"]) {
                for(auto const& v: node["used_vars"]) {
                    used_vars.insert(v.as<std::string>());
                }
            }
        }

        if(classifier.empty()) {
            for(auto const& f:{"S","F","R"})
                if(!SFR[f].empty())
                    classifier += std::string(f) + ": " + SFR[f] + ";\n";
        }

        auto c_func = expr.get_conditionary_function(SFR["S"], SFR["F"], SFR["R"]);
        add_to_builder(new Condition(id, builder.tree->get_memory(), c_func, used_vars, classifier));

        builder.view_graph[id]["class"] = std::any(classifier);
    }
}