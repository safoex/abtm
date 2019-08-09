//
// Created by safoex on 09.08.19.
//

#ifndef ABTM_ROSTOPIC_H
#define ABTM_ROSTOPIC_H

#include "../IOBase.h"
#include "rosbridge_ws_client.hpp"
#include <sstream>

namespace bt {
    class ROSTopic : public IOBase {
    public:
        enum ROSTOPIC_TYPE {
            Publisher,
            Subscriber
        };
    protected:
        RosbridgeWsClient& rbc;
        std::string topic_name;
        std::string msg_key;
        std::string client;
        std::unordered_set<std::string> trigger_keys;
        ROSTOPIC_TYPE type;
    public:
        ROSTopic(RosbridgeWsClient& rbc,  ROSTOPIC_TYPE type, std::string const& topic_name,
                std::string const& msg_key, std::unordered_set<std::string> trigger_keys = {}) : rbc(rbc), topic_name(topic_name),
                trigger_keys(trigger_keys), msg_key(msg_key), type(type), client(topic_name) {
            if(trigger_keys.empty())
                trigger_keys = {msg_key};
            sample trigger_vars;
            for(auto const& k: trigger_keys)
                trigger_vars[k] = double(0);
            set_vars({{msg_key, double(0)}}, trigger_vars);
            rbc.addClient(client);
        };


        virtual ~ROSTopic() {
            rbc.stopClient(client);
        };
    };


    class ROSPublisher : public ROSTopic {
    public:
        ROSPublisher (RosbridgeWsClient& rbc,  std::string const& topic_name, std::string const& msg_type,
                      std::string const& msg_key, std::unordered_set<std::string> const& trigger_keys = {})
                      : ROSTopic(rbc, Publisher, topic_name, msg_key, trigger_keys){
            rbc.advertise(client, topic_name, msg_type);
        }

        sample process(sample const& output) override {
            std::string msg_str;
            if(output.count(msg_key)) {
                try {
                    msg_str = std::any_cast<std::string>(output.at(msg_key));
                }
                catch (std::bad_any_cast &e) {
                    throw std::runtime_error("Key " + msg_key + " in sample for topic " + topic_name +" does not contain a std::string");
                }
            }
            rapidjson::Document d;
            d.Parse(msg_str.c_str());
            rbc.publish(topic_name, d);
            return {};
        };

    };

    class ROSSubscriber : public ROSTopic {
        InputFunction f;
    public:
        ROSSubscriber(RosbridgeWsClient& rbc, std::string const& topic_name,
        std::string const& msg_key) : ROSTopic(rbc, Subscriber, topic_name, msg_key, {}) {
            rbc.subscribe(client, topic_name, [this](
                    std::shared_ptr<WsClient::Connection> /* connection */, std::shared_ptr<WsClient::Message> message)
            {
                rapidjson::Document d;
                std::string msg(message->string());
                d.Parse(msg.c_str());

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                d["msg"].Accept(writer);

                std::cout << buffer.GetString() << std::endl;
                f({{this->msg_key, buffer.GetString()}});
            });
        }

        void set_subscriber(InputFunction const& from_mimo) {
            f = from_mimo;
        }
    };
}

#endif //ABTM_ROSTOPIC_H
