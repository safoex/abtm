//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_BASEPARSER_H
#define ABTM_BASEPARSER_H

#include <Node.h>
#include <Tree.h>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "Builder.h"

namespace bt {

    template <typename T>
    T load(const YAML::Node &node, std::string const &key) {
        if(node[key]) {
            try {
                return node[key].as<T>();
            }
            catch(YAML::Exception &e){
                throw YAML::Exception(YAML::Mark::null_mark(), std::string() + "Error while reading " + key + " " + e.what());
            }
        }
        return T();
    }

    template <typename T>
    T& load(const YAML::Node &node, std::string const &key, T& to) {
        if(node[key]) {
            try {
                to = node[key].as<T>();
            }
            catch(YAML::Exception &e){
                throw YAML::Exception(YAML::Mark::null_mark(), std::string() + "Error while reading " + key + " " + e.what());
            }
        }
        return to;
    }

    class BaseParser {
    public:
        Builder& builder;

        std::vector<std::string> requirements; // what should be parsed before, e.g. "variables" before "nodes"
        std::vector<std::string> optional_requirements;

        explicit BaseParser(Builder& builder) : builder(builder){};

        virtual void parse(std::string const& id, YAML::Node const &yaml_node) = 0;

        virtual ~BaseParser() = default;

    };
};

#endif //ABTM_BASEPARSER_H
