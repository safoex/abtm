//
// Created by safoex on 03.08.19.
//

#include "ConditionJSParser.h"
#include "esprima.h"

namespace bt{
    ConditionJSParser::ConditionJSParser(bt::Builder &builder, VariablesInNodeParser* vparser) : LeafJSParser(builder, vparser) {}


    void ConditionJSParser::parse(std::string const &id, YAML::Node const &node) {

        LeafJSParser::parse(id, node);

        std::set<std::string> sfr{"S","F","R"};
        Tree::dict<std::string> SFR{{"S",""},{"F",""},{"R",""}};

        std::unordered_set<std::string> used_vars;

        for(auto const& f: node) {
            if(sfr.count(f.first.as<std::string>()))
                SFR[f.first.as<std::string>()] = f.second.as<std::string>();
        }

        bool all_tested = true;
        std::stringstream err_msg;
        for(auto const& lr: SFR) {
            if(lr.second != "default" && !lr.second.empty()) {
                bool valid = builder.tree->get_memory().test_expr(lr.second);
                if (!valid) {
                    err_msg << "\tInvalid block: " << lr.first << ": " << lr.second << std::endl;
                }
                all_tested &= valid;
            }
        }

        if(!all_tested) {
            throw std::runtime_error("Error while loading action " + id + "\n" + err_msg.str());
        }
        // used vars
        if(!node["used_vars"]) {
            for(auto const& lr: SFR) {
                if (lr.second != "default" && !lr.second.empty()) {
                    auto uv = get_used_vars_from_expr(lr.second);
                    std::cout << "\t PARSED " << lr.second << std::endl;
                    std::cout << "\t GOT: \n";
                    for(auto const& v: uv) {
                        std::cout << "\t\t " << v << std::endl;
                    }
                    used_vars.insert(uv.begin(),uv.end());
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

        auto c_func = [SFR](MemoryBase& m) -> Node::State {
            dictOf <Node::State> states = {{"S", Node::SUCCESS},
                                           {"F", Node::FAILED},
                                           {"R", Node::RUNNING}};
            for (std::string const &st: {"F", "S", "R"}) {
                auto const &expr = SFR.at(st);
                if (!expr.empty() && expr != "default" && m.eval_bool(expr))
                    return states[st];
            }
            for (std::string const &st: {"F", "S", "R"}) {
                auto const &expr = SFR.at(st);
                if (expr == "default")
                    return states[st];
            }
            return Node::RUNNING;
        };

        add_to_builder(id, new Condition(id, builder.tree->get_memory(), c_func, used_vars, classifier));

        builder.view_graph[id]["class"] = classifier;
        builder.view_graph[id]["type"]  = std::string("condition");
        classifier = "";
    }
}