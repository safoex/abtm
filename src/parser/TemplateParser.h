//
// Created by safoex on 15.07.19.
//

#ifndef ABTM_TEMPLATEPARSER_H
#define ABTM_TEMPLATEPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "BaseParser.h"
#include "NodesParser.h"


namespace bt {
    class TemplateParser : public BaseParser {
    protected:
        void load_template(std::string const& id, YAML::Node const& yaml_node);
        void load_template_node(std::string const& id, YAML::Node const& yaml_node);
        const std::string name_symbol = "~", var_symbol = "$", var_symbol_regex = "\\$";
        const std::string alias_prefixes[2] = {"template/", "t/"};
        bool use_aliases;
        Parser* parser;
        NodesParser* nodesParser;
        YAML::Node reqursively_replace(YAML::Node const& yn, dictOf<YAML::Node> const& rep_args);
    public:
        TemplateParser(Builder& builder, NodesParser* nodesParser, Parser* parser, bool use_aliases = true);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~TemplateParser() override = default;

    };
}

#endif //ABTM_TEMPLATEPARSER_H
