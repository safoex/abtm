//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_PARSER_H
#define ABTM_PARSER_H

#include <Node.h>
#include <Tree.h>
#include <vector>
#include <unordered_map>
#include "ParserWithModules.h"
#include "Builder.h"
#include <yaml-cpp/yaml.h>
#include <defs.h>
#include <any>

namespace bt {

    class Parser : public ParserWithModules {
    protected:
        std::vector<std::string> sort_parsers();

    public:
        explicit Parser(Builder& builder);

        explicit Parser(Builder& builder, std::vector<std::pair<std::vector<std::string>, BaseParser*>> const& parsers);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        void parseMap(YAML::Node const& yaml_map);

        void loadYamlFile(std::string const& filename);
    };
};

#endif //ABTM_PARSER_H
