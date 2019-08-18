//
// Created by safoex on 09.08.19.
//

#ifndef ABTM_ROSPARSER_H
#define ABTM_ROSPARSER_H



#include "../IOParser.h"
#include "ROSTopic.h"

namespace bt {
    class ROSParser : public IOParser {
        friend class ROSTopic;
        friend class ROSPublisher;
        friend class ROSSubscriber;
    protected:
        RosbridgeWsClient rbc;
        std::string client;
        std::vector<ROSTopic*> topics;
    public:
        ROSParser(Builder& builder, MIMOCenter& mimo,
                std::string const& rosbridge_port = "localhost:9090",
                std::string const& client_name = "")
        : rbc(rosbridge_port), IOParser(builder, mimo) {
            if(client_name.empty())
                client = builder.tree->get_root_name();
            else
                client = client_name;
            rbc.addClient(client);
        };


        void parse(std::string const& id, YAML::Node const &yaml_node) override {
            auto const& yn = yaml_node;
            if(yn.IsMap()) {
                for(auto const& p: yn) {
                    if(p.second["type"]) {
                        std::string type;
                        try{
                            type = p.second["type"].as<std::string>();
                        }
                        catch(std::exception &e) {
                            throw std::runtime_error("type should be a string in " + p.first.as<std::string>() + " in ROS description");
                        }
                        if(type == "subscriber" || type == "sub" || type == "publisher" || type == "pub") {
                            std::string msg_key, msg_type, trigger_key;
                            std::unordered_set<std::string> trigger_keys;
                            load<std::string>(p.second, "var", msg_key);
                            load<std::string>(p.second, "msg", msg_type);

                            if(p.second["trigger"] && p.second["trigger"].IsSequence())
                                for(auto const& tv: p.second["trigger"]) {
                                    trigger_keys.insert(tv.as<std::string>());
                                }
                            else
                                load<std::string>(p.second, "trigger", trigger_key);
                            if(trigger_keys.empty()) {
                                if(trigger_key.empty())
                                    trigger_keys.insert(msg_key);
                                else
                                    trigger_keys.insert(trigger_key);
                            }
                            std::cout <<"ROS " << type << '\t' << p.first.as<std::string>() << '\t' << msg_key << std::endl;
                            if(type == "subscriber" || type == "sub") {
                                auto sub = new ROSSubscriber(rbc, p.first.as<std::string>(), msg_key);
                                sub->set_subscriber(mimo.registerIOchannel(sub));
                                topics.push_back(sub);
                            }
                            else if(type == "publisher" || type == "pub") {
                                auto pub = new ROSPublisher(rbc, p.first.as<std::string>(), msg_type, msg_key, trigger_keys);
                                mimo.registerIOchannel(pub);
                                topics.push_back(pub);
                            }
                        }
                        else throw std::runtime_error("Not supported handler \"" + type + "\" in ROS description");

                    }
                    else throw std::runtime_error("No type for " + p.first.as<std::string>() + " in ROS description");
                }
            }
            else throw std::runtime_error("ROS description should be a Map");
        }

        virtual ~ROSParser() {
            for(auto t:topics)
                delete t;
        };
    };
}



#endif //ABTM_ROSPARSER_H
