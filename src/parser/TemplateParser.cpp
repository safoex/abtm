//
// Created by safoex on 15.07.19.
//

#include "TemplateParser.h"
#include <regex>

namespace bt {
    TemplateParser::TemplateParser(bt::Builder &builder, NodesParser* nodesParser, Parser* parser, bool use_aliases)
        : BaseParser(builder), nodesParser(nodesParser), use_aliases(use_aliases), parser(parser) {
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
            if(yn["args"]) {
                if((yn["args"]["required"] && yn["args"]["required"]["type"]) ||
                    (yn["args"]["optional"] && yn["args"]["optional"]["type"]))
                    throw std::runtime_error("Error in template " + id + ": you could not name argument \"type\"");
            }
            auto& templates = std::any_cast<dictOf<std::string>&>(builder.extra["templates"]);
            templates[id] = ss.str();
            if(use_aliases)
                for(auto const& alias: alias_prefixes)
                    nodesParser->registerModule(alias + id, this);
//            std::cout << ss.str() << std::endl;
        }
        catch (std::exception& e) {
            throw std::runtime_error(std::string("Error occured while loading template ") + id + ": " + e.what());
        }
    }

    void TemplateParser::load_template_node(std::string const &id, YAML::Node const &yaml_node) {
        auto const& yn = yaml_node;
        try {
            auto& templates = std::any_cast<dictOf<std::string>&>(builder.extra["templates"]);
            //if it's type has template alias:
            std::string template_class;
            auto const& type = yn["type"].as<std::string>();
            for(auto const& a: alias_prefixes) {
                if(a.length() < type.length() && type.substr(0,a.length()) == a) {
                    template_class = type.substr(a.length());
                    break;
                }
            }
            if(template_class.empty())
                load<std::string>(yn, "class", template_class);
            if(template_class.empty())
                throw std::runtime_error(std::string("No template class provided for templated node ") + id);

            auto t = std::regex_replace(templates[template_class], std::regex(name_symbol), "_" + id + "_");
            YAML::Node t_fixed = YAML::Load(t);

            dictOf<std::string> replace_args, replace_args_view;
            replace_args["name"] = id;
            if(t_fixed["args"]) {
                auto const& t_args = t_fixed["args"];
                if(t_args["required"])
                    for(auto const& ra: t_args["required"]) {
                        std::string const& arg = ra.as<std::string>();
                        if(yn[arg])
                            replace_args[arg] = yn[arg].as<std::string>(),
                            replace_args_view[arg] = replace_args[arg];
                        else
                            throw std::runtime_error("No argument " + arg + " by templated node " + id);
                    }
                if(t_args["optional"]) {
                    for(auto const& oa: t_args["optional"]) {
                        std::string const& arg = oa.first.as < std::string >();
                        if(yn[arg])
                            replace_args[arg] = yn[arg].as<std::string>(),
                            replace_args_view[arg] = replace_args[arg];
                        else
                            replace_args[arg] = oa.second.as< std::string >();
                    }
                }
            }
            std::stringstream ss;
            ss << t_fixed;
            auto t_str = ss.str();

            for(auto const& a: replace_args) {
                auto const& _to = a.second, arg = a.first;
                auto from = var_symbol + arg;
                auto to = _to;
//                std::cout << "FROM " << from << " TO " << to << std::endl;
                t_str = std::regex_replace(t_str, std::regex(from), to);
            }
//            std::cout << t_str << std::endl;

            t_fixed = YAML::Load(t_str);

            // load extra variables
            if(t_fixed["var"]) {
                std::cout << "adding var for " + id << std::endl;
                parser->parse("add", t_fixed["var"]);
            }

            // load nodes
            nodesParser->parse("nodes", t_fixed["nodes"]);


            // get fake children for visualization
            std::vector<std::string> view_children;
            if(t_fixed["children"])
                for(auto const& c: t_fixed["children"])
                    view_children.push_back(c.as<std::string>());
            builder.view_graph[id]["children"] = view_children;
            builder.view_graph[id]["type"] = type;

            // get hide parameter for visualization (if true, then hide children)
            if(yaml_node["hide"])
                builder.view_graph[id]["hide"] = load<int>(yaml_node, "hide");

            std::string default_template_color = "\"#9262d1\"";
            builder.view_graph[id]["color"] = load(yaml_node, "color", default_template_color);

            std::string classifier = type + '\n';
            for(auto const& pa: replace_args_view)
                classifier += (std::string(pa.first) + ": " + pa.second + "\n");

            builder.view_graph[id]["class"] = classifier;

            for(auto const& n: t_fixed["nodes"]) {
                auto const& node = n.first.as<std::string>();
                bool is_in_view_children = false;
                for(auto const& vc: view_children)
                    if(node == vc)
                        is_in_view_children = true;
                if(!is_in_view_children && node != id)
                    builder.view_graph.erase(node);
            }

        }
        catch (std::exception& e) {
            throw std::runtime_error("Error while parsing node " + id + ": " + e.what());
        }
    }


}