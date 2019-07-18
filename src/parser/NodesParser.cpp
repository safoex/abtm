//
// Created by safoex on 15.07.19.
//

#include "NodesParser.h"

namespace bt{
    NodesParser::NodesParser(bt::Builder &builder) : ParserWithModules(builder) {
        optional_requirements.emplace_back("templates");
    }

    NodesParser::NodesParser(bt::Builder &builder,
         const std::vector<std::pair<std::vector<std::string>, bt::BaseParser *>> &parsers)
         : ParserWithModules(builder, parsers) {
        optional_requirements.emplace_back("templates");
    }

    void NodesParser::parse(std::string const &id, YAML::Node const &yaml_node) {

        auto const& node = yaml_node;

        if(!node.IsMap()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "Not a map! : " + id);
        }

        for(auto const& p:yaml_node) {
            try {
                // node id (name)
                std::string const &name = p.first.as<std::string>();

                // node type
                std::string const &type = p.second["type"].as<std::string>();

                if (parsers.count(type)) {
                    std::cout << name << ' ' << type << std::endl;
                    parsers[type]->parse(name, p.second);
                } else throw std::runtime_error(std::string("No parser provided for node") + name + " type: " + type);
            }
            catch(std::exception& e) {
                throw std::runtime_error(std::string("Error while loading node ") + p.first.as<std::string>() + " " + e.what());
            }
        }
    }

}