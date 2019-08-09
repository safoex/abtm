//
// Created by safoex on 03.08.19.
//

#include "ActionJSParser.h"

namespace bt {

    ActionJSParser::ActionJSParser(bt::Builder &builder, VariablesInNodeParser* vparser) : LeafJSParser(builder, vparser) {}

    void ActionJSParser::parse(std::string const &id, YAML::Node const &node) {

        LeafJSParser::parse(id, node);

        if( !(node["assign"] || node["script"]) ) {
            throw std::runtime_error("Now script or assign provided for action node " + id);
        }

        if(node["assign"] && node["script"]) {
            throw std::runtime_error("Both script or assign provided for action node " + id);
        }

        Action::ActionaryFunction af;
        if(node["assign"]) {
            if (!node["assign"].IsMap()) {
                throw YAML::Exception(YAML::Mark::null_mark(), "Assign is not a map in node  " + id);
            }
            auto const &assign = node["assign"];
            dictOf<std::string> lrvalues;
            for(auto const& p: assign) {
                lrvalues[p.first.as<std::string>()] = p.second.as<std::string>();
            }

            // test expressions:
            bool all_tested = true;
            std::stringstream err_msg;
            for(auto const& lr: lrvalues) {
                bool valid = builder.tree->get_memory().test_expr(lr.first + " = " + lr.second);
                classifier += lr.first + ": " + lr.second + '\n';
                if(!valid) {
                    err_msg << "\tInvalid assign block: " << lr.first << ": " << lr.second << std::endl;
                }
                all_tested &= valid;
            }

            if(!all_tested) {
                throw std::runtime_error("Error while loading action " + id + "\n" + err_msg.str());
            }


            af = [lrvalues](MemoryBase &m) {
                for (auto const &lr: lrvalues)
                    m.set_expr(lr.first, lr.second);
            };

        }
        else {
            if( !node["script"].IsScalar()) {
                throw std::runtime_error("Script is not a string in node " + id);
            }
            auto script = node["script"].as<std::string>();
            classifier = script;
            af = [script](MemoryBase &m) {
                m.eval_action(script);
            };

        }

        add_to_builder(id, new Action(id, builder.tree->get_memory(), af, {}, classifier));

        builder.view_graph[id]["class"] = classifier;
        builder.view_graph[id]["type"]  = std::string("action");
        classifier = "";
    }
}