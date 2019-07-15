//
// Created by safoex on 09.07.19.
//

#include "ActionStrParser.h"

namespace bt {
    using namespace expr;

    ActionStrParser::ActionStrParser(bt::Builder &builder) : LeafStrParser(builder) {}

    void ActionStrParser::parse(std::string const &id, YAML::Node const &node) {
        LeafStrParser::parse(id, node);


        std::unordered_set<std::string> used_vars;

        if(!node["assign"].IsMap()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "Not a map! : " + id);
        }
        auto const& assign = node["assign"];
        std::vector<Action::ActionaryFunction> assignments;
        dictOf<std::string> lrvalues;
        bool add_classifier = (classifier.empty());
        try {
            for (auto const &p: assign) {
                auto const &lvalue = p.first.as<std::string>();
                std::string rvalue = p.second.as<std::string>();
                assignments.push_back(expr.get_actionary_function(lvalue, rvalue));
                lrvalues[lvalue] = rvalue;
                if(add_classifier)
                    classifier += lvalue + " := " + rvalue + ";\n";
                used_vars.insert(lvalue);
                auto uv = find_vars(rvalue);
                used_vars.insert(uv.begin(), uv.end());
            }
        }
        catch(YAML::Exception & e) {
            throw YAML::Exception(YAML::Mark::null_mark(), "failed assignments load at action " + id + " exc:" + e.what());
        }

        auto a_func = [assignments](Memory<double>& m) {
            for(auto const& f: assignments)
                f(m);
        };

        add_to_builder(id, new Action(id, builder.tree->get_memory(), a_func, {}, classifier));

        builder.view_graph[id]["class"] = std::any(classifier);
    }
}