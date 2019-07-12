//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_PARSER_H
#define ABTM_PARSER_H

#include <Node.h>
#include <Tree.h>
#include <vector>
#include <unordered_map>
#include "BaseParser.h"
#include "Builder.h"
#include <yaml-cpp/yaml.h>
#include <defs.h>
#include <any>

namespace bt {

    class Parser {
        dictOf<BaseParser*> parsers; // {scope e.g. "nodes", parser [...]}
        Builder builder;
        std::string root_node;
        std::vector<std::string> sort_parsers();
    public:
        Parser(Builder builder);
        Parser(Tree* tree);
        void registerModule(std::string const& type, BaseParser *parser);
        void loadYamlFile(std::string const& filename);
        virtual ~Parser();
    };
};

#endif //ABTM_PARSER_H
