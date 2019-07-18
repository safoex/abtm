//
// Created by safoex on 15.07.19.
//

#include "TemplateParser.h"
#include <regex>

namespace bt {
    TemplateParser::TemplateParser(bt::Builder &builder, NodesParser* nodesParser)
        : BaseParser(builder), nodesParser(nodesParser) {
        if(!builder.extra["templates"].has_value())
            builder.extra["templates"] = dictOf<std::string>();
    }

    void TemplateParser::parse(std::string const &id, YAML::Node const &yaml_node) {
        if(id == "templates")
            for(auto const& n: yaml_node) {
                load_template(n.first.as<std::string>(), n.second);
            }
        else
            load_template_node(id, yaml_node);
    }

    void TemplateParser::load_template(std::string const &id, YAML::Node const &yaml_node) {
        auto const& yn = yaml_node;
        try {
            std::stringstream ss;
            ss << yn;
            auto& templates = std::any_cast<dictOf<std::string>&>(builder.extra["templates"]);
            templates[id] = ss.str();
            std::cout << ss.str() << std::endl;
        }
        catch (std::exception& e) {
            throw std::runtime_error(std::string("Error occured while loading template ") + id + ": " + e.what());
        }
    }

    void TemplateParser::load_template_node(std::string const &id, YAML::Node const &yaml_node) {
        auto const& yn = yaml_node;

        try {
            auto& templates = std::any_cast<dictOf<std::string>&>(builder.extra["templates"]);
            std::string template_class;
            load<std::string>(yn, "class", template_class);
            if(template_class.empty())
                throw std::runtime_error(std::string("No template class provided for templated node ") + id);

            std::cout << templates[template_class] << std:: endl << std::endl;
            auto t = std::regex_replace(templates[template_class], std::regex(name_symbol), "_" + id + "_");
            YAML::Node t_fixed = YAML::Load(t);
            std::cout << t << std:: endl << std::endl;
            dictOf<std::string> replace_args;
            replace_args["name"] = id;
            if(t_fixed["args"]) {
                auto const& t_args = t_fixed["args"];
                if(!yn["args"] && t_args["required"])
                    throw std::runtime_error(std::string("No args provided by templated node ") + id);
                if(t_args["required"])
                    for(auto const& ra: t_args["required"]) {
                        std::string const& arg = ra.as<std::string>();
                        if(yn["args"][arg])
                            replace_args[arg] = yn["args"][arg].as<std::string>();
                        else
                            throw std::runtime_error("No argument " + arg + " by templated node " + id);
                    }
                if(t_args["optional"]) {
                    for(auto const& oa: t_args["optional"]) {
                        std::string const& arg = oa.first.as < std::string >();
                        if(yn["args"][arg])
                            replace_args[arg] = yn["args"][arg].as<std::string>();
                        else
                            replace_args[arg] = oa.second.as< std::string >();
                    }
                }
            }
            std::stringstream ss;
            ss << t_fixed["nodes"];
            auto t_str = ss.str();

            for(auto const& a: replace_args) {
                auto const& _to = a.second, arg = a.first;
//                auto from = var_symbol + arg + R"(\w)";
//                auto to   = _to + "\\2";
                auto from = var_symbol + arg;
                auto to = _to;
                std::cout << "FROM " << from << " TO " << to << std::endl;
                t_str = std::regex_replace(t_str, std::regex(from), to);
            }
            std::cout << t_str << std::endl;

            t_fixed = YAML::Load(t_str);

            nodesParser->parse("nodes", t_fixed);

        }
        catch (std::exception& e) {
            throw std::runtime_error("Error while parsing node " + id + ": " + e.what());
        }
    }


}