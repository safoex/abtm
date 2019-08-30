//
// Created by safoex on 19.11.18.
//

#include "Node.h"
#include "Memory.cpp"
//#include "std_string.h"
#include <iostream>

#define LOGGING false

namespace bt {

    Node::TickType Parallel::call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY] = {
            {TOPDOWN_FALL, BOTTOMUP_FALL, NO_TICK, BOTTOMUP_FALL, DEACTIVATION_RUN, DEACTIVATION_RUN   },
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK, NO_TICK}
    };

    Node::TickType RunningSkippingSequence::call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY] = {
            {TOPDOWN_FALL, TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL, DEACTIVATION_RUN, DEACTIVATION_RUN   },
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK, NO_TICK}
    };

    Node::TickType Sequential::call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY] = {
        //A_F,          A_R,            C_F,        C_R,        D_S,                  D_F
            {TOPDOWN_FALL, TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL, DEACTIVATION_RUN, DEACTIVATION_RUN   },
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK},
            {TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL, NO_TICK, NO_TICK, NO_TICK}
    };

    Node::TickType Node::return_tick_table[FINAL_STATE_ENTRY+1][FINAL_STATE_ENTRY+1] = { //i - changed from, j - changed to
        {NO_TICK, TOPDOWN_RISE, TOPDOWN_RISE, NO_TICK},
        {NO_TICK, NO_TICK, BOTTOMUP_RISE, NO_TICK},
        {NO_TICK, BOTTOMUP_RISE, NO_TICK, NO_TICK},
        {NO_TICK, NO_TICK, NO_TICK, NO_TICK}
    };


    const auto _STATE = std::vector<std::string>{"RUNNING", "SUCCESS", "FAILURE", "UNDEFINED"};
    std::string STATE(int x) {
        return _STATE[x];
    }

    const auto _TICK_TYPE = std::vector<std::string>{"TOPDOWN_FALL", "TOPDOWN_RISE", "BOTTOMUP_FALL", "BOTTOMUP_RISE",
                                                     "DEACTIVATION_RUN", "DEACTIVATION_AFTER", "NO_TICK"};
    std::string TICK_TYPE(int x) {
        return _TICK_TYPE[x];
    }

    const auto _NODE_CLASS = std::vector<std::string>{"Sequence",
                                                      "Selector",
                                                      "Parallel",
                                                      "RunningSkippingSequence",
                                                      "SFRselector",
                                                      "Latch",
                                                      "Action",
                                                      "Condition"};
    std::string NODE_CLASS(int x) {
        return _NODE_CLASS[x];
    }

    std::string Node::state_var() const {
        return "__STATE__" + id();
    }

    Node::State Node::state() const {
        return Node::State(vars.get_double(state_var()).value());
    }


    Node::Node(std::string id, MemoryBase& vars, std::string classifier) : _id(id), vars(vars),
    _classifier(classifier), hide_further(false), visited(false) {
        vars.add(state_var(), VarScope::INNER, double(Node::UNDEFINED));
    }

    Node::~Node() {}

    const std::string& Node::id() const {
        return _id;
    }

    const std::string& Node::classifier() const {
        return _classifier;
    }

    Node::tick_return_type Node::return_tick(bt::Node::State before, bt::Node::State after) const {
        return std::make_pair(after, return_tick_table[before][after]);
    }

    Sequential::Sequential(std::string id, bt::MemoryBase &vars, std::string classifier) : Node(id,vars,classifier) {}

    void Sequential::add_child(Node *child) {
        if (children.size() < children_limit || children_limit < 0)
            children.push_back(child);
    }

    void Sequential::insert_child(bt::Node *child, std::string const& child_name, bool after) {
        if (children.size() < children_limit || children_limit < 0) {
            if(child_name.empty()) {
                if(after) {
                    children.push_back(child);
                }
                else {
                    children.push_front(child);
                }
            }
            else {
                auto it_found = children.end();
                for (auto it = children.begin(); it != children.end(); it++) {
                    if ((*it)->id() == child_name) {
                        it_found = it;
                        break;
                    }
                }
                if(it_found != children.end())
                    after ? children.insert(++it_found,child) : children.insert(it_found, child);
                else
                    children.insert(it_found, child);
            }
        }
    }

    void Sequential::remove_child(std::string const &child_name) {
        auto it_found = children.end();
        for (auto it = children.begin(); it != children.end(); it++) {
            if ((*it)->id() == child_name) {
                it_found = it;
                break;
            }
        }
        if(it_found != children.end())
            children.erase(it_found);
    }

    void Sequential::start_deactivation(bt::Node::TickType tick_type, bt::Node::tick_return_type tick_return) {
        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER) {
            if (state() == RUNNING) {
                evaluate(DEACTIVATION_AFTER);
            } else evaluate(DEACTIVATION_RUN);
        }
    }

    Node::tick_return_type Sequential::tick(TickType tick_type) {
        auto tick_children = get_call_table(state(), tick_type);
        auto _old_state = state();
        if(visited && false)
            tick_children = NO_TICK;
//        std::cout << "tick " << id() << ' ' << STATE(state()) << ' ' << TICK_TYPE(tick_type) << std::endl;
        vars.set(state_var(), (double)evaluate(tick_children));
        start_deactivation(tick_type, return_tick(_old_state, state()));

        visited = true;
        return return_tick(_old_state, state());
    }

    Node::TickType Sequential::get_call_table(bt::Node::State s, bt::Node::TickType t) {
        return Sequential::call_table[s][t];
    }


    Sequential& Sequential::operator=(const Sequential& other) {
        children = other.children;
        return *this;
    }

    Sequential::~Sequential() {}

    const std::list<Node*>& Sequential::get_children() const {
        return children;
    }

    int Sequential::children_size() const {
        return children.size();
    }

    Selector::Selector(std::string id, MemoryBase& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State Sequential::evaluate(TickType tick_type) {
        if(tick_type == NO_TICK)
            return state();
//        std::cout << "eval " <<  id() << ' ' << TICK_TYPE(tick_type) << std::endl;
        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER) {
            for (auto &child : children) {
                auto status = child->tick(tick_type);
                if (status.first != return_state()) return status.first;
            }
        }
        else {
            bool kill_since_next = tick_type == DEACTIVATION_RUN;
            for (auto &child : children) {
                if(!kill_since_next) {
                    auto status = child->state();
                    if (status != return_state())
                        kill_since_next = true;
                }
                else child->tick(DEACTIVATION_RUN);
            }
            if(tick_type == DEACTIVATION_RUN) return UNDEFINED;
            else return state();
        }

        return return_state();
    }


    Node::NodeClass Selector::node_class() const {
        return Node::Selector;
    }

    Node::State Selector::return_state() { return Node::FAILED;}

    Sequence::Sequence(std::string id, MemoryBase& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::NodeClass Sequence::node_class() const {
        return Node::Sequence;
    }

    Node::State Sequence::return_state() { return Node::SUCCESS;}

    Parallel::Parallel(std::string id, MemoryBase& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State Parallel::return_state() {return SUCCESS;}

    Node::State Parallel::evaluate(TickType tick_type) {
        if(tick_type == NO_TICK)
            return state();

        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER) {
            int states[State::FINAL_STATE_ENTRY + 1]{0};
            for (auto &child: children) {
                states[child->tick(tick_type).first]++;
            }
            if (states[FAILED] > 0) return FAILED;
            if (states[RUNNING] > 0) return RUNNING;
            return SUCCESS;
        }
        else if(tick_type == DEACTIVATION_RUN) {
            for(auto &child: children) {
                child->tick(DEACTIVATION_RUN);
            }
            if(state() == RUNNING)
                return UNDEFINED;
            else return state();
        }
        return state();
    }

    Node::TickType Parallel::get_call_table(bt::Node::State s, bt::Node::TickType t) {
        return Parallel::call_table[s][t];
    }

    Node::NodeClass Parallel::node_class() const {
        return Node::Parallel;
    }

    RunningSkippingSequence::RunningSkippingSequence(std::string id, bt::MemoryBase &vars,
                                                     std::string classifier) : Sequential(id, vars, classifier) {}

    Node::NodeClass RunningSkippingSequence::node_class() const {
        return Node::RunningSkippingSequence;
    }

    Node::TickType RunningSkippingSequence::get_call_table(bt::Node::State s, bt::Node::TickType t) {
        return RunningSkippingSequence::call_table[s][t];
    }

    void RunningSkippingSequence::start_deactivation(bt::Node::TickType tick_type,
                                                     bt::Node::tick_return_type tick_return) {
        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER)
            if(state() != RUNNING) evaluate(DEACTIVATION_RUN);
    }

    Node::State RunningSkippingSequence::return_state() { return RUNNING;}


    const std::unordered_set<std::string>& ActiveNode::get_used_vars() const {
        return used_vars;
    }

    ActiveNode::ActiveNode(std::string id, MemoryBase& vars, std::string classifier,
            std::unordered_set<std::string> const &used_vars) : NoChildNode(id, vars, classifier), used_vars(used_vars) {}

    ActiveNode::~ActiveNode() {}

    const std::list<Node*>& NoChildNode::get_children() const {
        return std::list<Node*>();
    }

    int NoChildNode::children_size() const {
        return 0;
    }

    NoChildNode::NoChildNode(std::string id, bt::MemoryBase &vars, std::string classifier) : Node(id, vars, classifier) {}

    Condition::Condition(std::string id, MemoryBase &vars, const bt::Condition::ConditionaryFunction &function,
                         std::unordered_set<std::string> const &used_vars, std::string classifier):
            ActiveNode(id, vars, classifier, used_vars), function(function) {}

    Node::tick_return_type Condition::tick(TickType tick_type) {
        auto _old_state = state();
        if(visited && false)
            tick_type = NO_TICK;
        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER)
            vars.set(state_var(), (double)evaluate(tick_type));
        visited = true;
        return return_tick(_old_state, state());
    }

    Node::tick_return_type Condition::evaluate_and_return_tick_type(bt::Node::TickType tick_type)  {
        auto _old_state = state();
        auto _new_state = _old_state;
        if(visited && false)
            tick_type = NO_TICK;
        if(tick_type != DEACTIVATION_RUN && tick_type != DEACTIVATION_AFTER)
            _new_state = evaluate(tick_type);
        visited = true;
        return return_tick(_old_state, _new_state);
    }

    Node::State Condition::evaluate(TickType tick_type) {
        Node::State state(Node::RUNNING);
        try {
            state = function(vars);
        }
        catch (std::exception const& e) {
            DEBUG_PR("exception in evaluate "+ id());
        }
        return state;
    }


    Condition& Condition::operator=(const bt::Condition &other) {
        function = other.function;
        used_vars = other.used_vars;
        return *this;
    }

    Node::NodeClass Condition::node_class() const {
        return Node::Condition;
    }

    Action::Action(std::string id, bt::MemoryBase &vars, const bt::Action::ActionaryFunction &function,
                   std::unordered_set<std::string> const &used_vars,
                   std::string classifier): ActiveNode(id, vars, classifier, used_vars), function(function) {}

    Node::tick_return_type Action::tick(TickType tick_type) {
        auto _old_state = state();
        if(visited && false)
            tick_type = NO_TICK;
        vars.set(state_var(), (double)evaluate(tick_type));
        visited = true;
        return return_tick(_old_state, state());

    }
    Node::State Action::evaluate(TickType tick_type) {
        if(tick_type == Node::TOPDOWN_FALL) {
            try {
                function(vars);
            }
            catch(std::exception const& e) {
                DEBUG_PR("exception in action " + id());
            }
//            LOG_DEBUG("action " + classifier() + " called");
        }
        return Node::SUCCESS;
    }

    Action& Action::operator=(const bt::Action &other) {
        function = other.function;
        return *this;
    }



    Node::NodeClass Action::node_class() const {
        return Node::Action;
    }


};
