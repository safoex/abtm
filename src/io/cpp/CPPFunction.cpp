//
// Created by safoex on 26.07.19.
//

#include "CPPFunction.h"
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace bt {
    CPPFunction::CPPFunction(bt::ExternalFunction f, std::string name, bt::sample required_vars, unsigned priority)
        : f(f), name(name), required_vars(required_vars), priority(priority) {}

    YAML::Node CPPFunction::get_YAML_recipe() const {
        YAML::Emitter out;
        YAML::Node n;
        n["name"] = name;
        for(auto const&v : required_vars)
            n["vars"].push_back(v.first);
        return n;
    }

    std::string CPPFunction::get_yaml_recipe() const {
        std::stringstream ss;
        ss << get_YAML_recipe();
        return ss.str();
    }
}