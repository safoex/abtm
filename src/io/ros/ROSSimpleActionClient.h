//
// Created by safoex on 02.09.19.
//

#ifndef ABTM_SIMPLEACTIONCLIENT_H
#define ABTM_SIMPLEACTIONCLIENT_H
#include "../IOBase.h"
#include "rosbridge_ws_client.hpp"
#include <sstream>

namespace bt {

    class ROSSimpleActionClient : public IOBase {
    protected:
        std::string json_to_str(rapidjson::Document::GenericValue const& value) {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            value.Accept(writer);
            return buffer.GetString();
        }
    public:
        RosbridgeWsClient& rbc;
        std::string as_name, action, package;
        std::string simple_status_var;
        std::string goal_var, cancel_var, feedback_var, result_var;
        std::string goal_id, goal_id_var, GoalID;
        InputFunction process_input;

        std::string goal_topic, cancel_topic;

        ROSSimpleActionClient(RosbridgeWsClient& rbc, std::string const& _as_name, std::string const& _package,
                std::string const& _action, std::string const& _goal_var, std::string const& _cancel_var,
                std::string const& _feedback_var, std::string const& _result_var, std::string const&  _simple_status_var,
                std::string const& _goal_id_var) :
                rbc(rbc), as_name(_as_name), action(_action), simple_status_var(_simple_status_var), goal_var(_goal_var),
                cancel_var(_cancel_var), feedback_var(_feedback_var), result_var(_result_var), package(_package),
                goal_id_var(_goal_id_var) {


            goal_topic = _as_name + "/goal";
            cancel_topic = _as_name + "/cancel";
            rbc.addClient(goal_topic);
            rbc.advertise(goal_topic, goal_topic, package + "/" + action + "ActionGoal");
            rbc.addClient(cancel_topic);
            rbc.advertise(cancel_topic, cancel_topic, "actionlib_msgs/GoalID");

            auto feedback_cb = [this](std::shared_ptr<WsClient::Connection> /* connection */,
                    std::shared_ptr<WsClient::Message> message) {
                return this->onFeedback(message);
            };

            auto result_cb = [this](std::shared_ptr<WsClient::Connection> /* connection */,
                    std::shared_ptr<WsClient::Message> message) {
                return this->onResult(message);
            };

            auto status_cb = [this](std::shared_ptr<WsClient::Connection> /* connection */,
                    std::shared_ptr<WsClient::Message> message) {
                return this->onGoalStatusArray(message);
            };

            auto    status_topic    = _as_name + "/status",
                    feedback_topic  = _as_name + "/feedback",
                    result_topic    = _as_name + "/result";
            rbc.addClient(result_topic);
            rbc.addClient(status_topic);
            rbc.addClient(feedback_topic);
            rbc.subscribe(status_topic, status_topic,   status_cb);
            rbc.subscribe(feedback_topic, feedback_topic, feedback_cb);
            rbc.subscribe(result_topic, result_topic,   result_cb);
            goal_id = "";
            GoalID = "";

            sample trigger_vars{{cancel_var, double(0)},{goal_id_var, double(0)}};
            sample required_vars(trigger_vars);
            required_vars.insert({goal_var, double(0)});
            set_vars(required_vars, trigger_vars);
        }

        void set_process(InputFunction const& mimo_process){
            process_input = mimo_process;
        };

        sample process(sample const& output) override {
            for(auto [k,v]: output) {
                std::cout << k << '\t';
            }
            std::cout << std::endl;
            if(std::any_cast<std::string>(output.at(goal_id_var)) != goal_id) {
                GoalID = std::any_cast<std::string>(output.at(goal_id_var));
                rapidjson::Document g;
                g.Parse(GoalID.c_str());
                goal_id = g["id"].GetString();

                auto goal = std::any_cast<std::string>(output.at(goal_var));
                rapidjson::Document d;
                d.Parse(goal.c_str());
                rbc.publish(goal_topic, d);
                return {{simple_status_var, "{\"goal_id\": "+GoalID +", \"status\": 0,\"text\":\"\"}" }};
            }
            else {
                auto cancel = std::any_cast<std::string>(output.at(cancel_var));
                rapidjson::Document d;
                d.Parse(cancel.c_str());
                if(d["id"].GetString() == goal_id) {
                    rbc.publish(cancel_topic, d);
                }
                return {};
            }
        }

        ~ROSSimpleActionClient() override = default;

    protected:
        void onFeedback(std::shared_ptr<WsClient::Message> const& message) {
            rapidjson::Document d;
            std::string msg(message->string());
            d.Parse(msg.c_str());
//            std::cout << msg << std::endl;

            if(d["msg"]["status"]["goal_id"]["id"].GetString() == goal_id) {
                sample s{{simple_status_var,  json_to_str(d["msg"]["status"])},
                         {feedback_var,       json_to_str(d["msg"])}};
                print_sample(s);
                process_input(s);
            }
        }

        void onGoalStatusArray(std::shared_ptr<WsClient::Message> const& message) {
            rapidjson::Document d;
            std::string msg(message->string());
            d.Parse(msg.c_str());
//            std::cout << msg << std::endl;

            for(auto it = d["msg"]["status_list"].Begin(); it != d["msg"]["status_list"].End(); it++) {
                if((*it)["goal_id"]["id"].GetString() == goal_id) {
                    sample s{{simple_status_var, json_to_str((*it)["status"])}};
                    print_sample(s);
                    process_input(s);
                }
            }
        }

        void onResult(std::shared_ptr<WsClient::Message> const& message) {
            rapidjson::Document d;
            std::string msg(message->string());
            d.Parse(msg.c_str());
            std::cout << json_to_str(d["msg"]) << std::endl;
            std::cout << json_to_str(d["msg"]["status"]) << std::endl;
            std::cout << json_to_str(d["msg"]["status"]["goal_id"]) << std::endl;
            std::cout << d["msg"]["status"]["goal_id"]["id"].GetString() << std::endl;
            std::cout << goal_id << std::endl;

            if(d["msg"]["status"]["goal_id"]["id"].GetString() == goal_id) {
                sample s{{simple_status_var, json_to_str(d["msg"]["status"])},
                         {result_var,        json_to_str(d["msg"])}};
                print_sample(s);
                process_input(s);
            }
        }
    };

}

#endif //ABTM_SIMPLEACTIONCLIENT_H
