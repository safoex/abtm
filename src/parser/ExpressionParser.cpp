//
// Created by safoex on 09.07.19.
//

#include "ExpressionParser.h"

namespace bt {
    ExpressionParser::ExpressionParser(bt::Memory<double> &memory) : m(memory) {}

    Action::ActionaryFunction ExpressionParser::get_actionary_function(std::string const &lvalue,
                                                                   std::string const &rvalue) const {
        return [lvalue, rvalue, this](Memory<double> &m) {
            double rv = this->get_rvalue_function(rvalue)(m);
            LOG_DEBUG("set " + lvalue + " to " + std::to_string(rv));
            m.set(lvalue, rv);
        };
    }

    Condition::ConditionaryFunction ExpressionParser::get_conditionary_function(const std::string &success,
                                                                            const std::string &failed,
                                                                            const std::string &running) const {
        std::vector<std::string> funcs{failed, success, running};
        std::vector<Node::State> states{Node::FAILED, Node::SUCCESS, Node::RUNNING};
        return [funcs, states, this](const Memory<double> &m) -> Node::State {
            for (int i = 0; i < 3; i++) {
                auto const &f = funcs[i];
                if (!f.empty() && f != "default") {
                    if (this->get_rvalue_function(f)(m) != 0) {
                        return states[i];
                    }
                }
            }

            for (int i = 0; i < 3; i++) {
                if (funcs[i] == "default")
                    return states[i];
            }

            return Node::RUNNING;
        };
    }

    ExpressionParser::RValueFunction ExpressionParser::get_rvalue_function(std::string const &rvalue) const {
        return [rvalue](const Memory<double> &m) -> double {
            if(rvalue.empty()) return 0;
            if(rvalue == "default") return 1;
            return bt::calc(rvalue, m);
        };
    }
}

