//
// Created by safoex on 19.11.18.
//

#include "Node.h"
#include "Memory.cpp"
#include "std_string.h"
#include <iostream>

#define LOGGING false

namespace bt {

    Node::TickType Latch::call_table[3][4] = {
        {TOPDOWN_FALL, BOTTOMUP_FALL, NO_TICK, BOTTOMUP_FALL},
        {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL},
        {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL}
    };

    Node::TickType Sequential::call_table[3][4] = {
        {TOPDOWN_FALL, TOPDOWN_FALL, NO_TICK, BOTTOMUP_FALL},
        {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL},
        {TOPDOWN_FALL, NO_TICK, NO_TICK, BOTTOMUP_FALL}
    };

    Node::TickType Node::return_tick_table[3][3] = { //i - changed from, j - changed to
        {NO_TICK, TOPDOWN_RISE, TOPDOWN_RISE},
        {NO_TICK, NO_TICK, BOTTOMUP_RISE},
        {NO_TICK, BOTTOMUP_RISE, NO_TICK}
    };


    const auto _STATE = std::vector<std::string>{"RUNNING", "SUCCESS", "FAILURE"};
    std::string STATE(int x) {
        return _STATE[x];
    }

    const auto _TICK_TYPE = std::vector<std::string>{"TOPDOWN_FALL", "TOPDOWN_RISE", "BOTTOMUP_FALL", "BOTTOMUP_RISE", "NO_TICK"};
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
        return "@" + id();
    }

    Node::State Node::state() const {
        return Node::State(vars[state_var()]);
    }


    Node::Node(std::string id, Memory<double>& vars, std::string classifier) : _id(id), vars(vars),
    _classifier(classifier), hide_further(false) {
        vars.add_variable(state_var());
        vars.set(state_var(), Node::SUCCESS);
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

    Sequential::Sequential(std::string id, bt::Memory<double> &vars, std::string classifier) : Node(id,vars,classifier) {}

    void Sequential::add_child(Node *child) {
        if (children_limit > 0 || children.size() < children_limit)
            children.push_back(child);
    }

    Node::tick_return_type Sequential::tick(TickType tick_type) {
        if(LOGGING)
            DEBUG_PR(id() + ' ' + STATE(state()) + ' ' + TICK_TYPE(tick_type))


        auto tick_children = call_table[state()][tick_type];
        auto _old_state = state();
        vars.set(state_var(), evaluate(tick_children));
        return return_tick(_old_state, state());
    }

    Sequential& Sequential::operator=(const Sequential& other) {
        children = other.children;
        return *this;
    }

    Sequential::~Sequential() {}

    const std::vector<Node*>& Sequential::get_children() const {
        return children;
    }

    int Sequential::children_size() const {
        return children.size();
    }

    Selector::Selector(std::string id, Memory<double>& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State Selector::evaluate(TickType tick_type) {
        if(tick_type == NO_TICK)
            return state();
        for(auto &child : children) {
            auto status = child->tick(tick_type);
            if(status.first == SUCCESS || status.first == RUNNING) return status.first;
        }
        return FAILED;
    }

    Node::NodeClass Selector::node_class() const {
        return Node::Selector;
    }

    Sequence::Sequence(std::string id, Memory<double>& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State Sequence::evaluate(TickType tick_type) {
        if(LOGGING)
            DEBUG_PR("sequence "+id())
        if(tick_type == NO_TICK)
            return state();
        if(LOGGING)
            DEBUG_PR(children.size());
        for(auto &child : children) {
            if(LOGGING)
                DEBUG_PR("child " + child->id());
            auto status = child->tick(tick_type);
            if(status.first == RUNNING || status.first == FAILED) return status.first;
        }
        return SUCCESS;
    }

    Node::NodeClass Sequence::node_class() const {
        return Node::Sequence;
    }

    Parallel::Parallel(std::string id, Memory<double>& vars, std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State Parallel::evaluate(TickType tick_type) {
        if(tick_type == NO_TICK)
            return state();
        int states[State::FINAL_STATE_ENTRY + 1];
        for(auto &child: children) {
            states[child->tick(tick_type).first]++;
        }
        if(states[FAILED] > 0) return FAILED;
        if(states[RUNNING] > 0) return RUNNING;
        return SUCCESS;
    }

    Node::NodeClass Parallel::node_class() const {
        return Node::Parallel;
    }

    RunningSkippingSequence::RunningSkippingSequence(std::string id, bt::Memory<double> &vars,
                                                     std::string classifier) : Sequential(id, vars, classifier) {}

    Node::State RunningSkippingSequence::evaluate(bt::Node::TickType tick_type) {
        if(LOGGING)
            LOG_DEBUG("rss " + id() + " " + STATE(state()));
        if(tick_type == NO_TICK)
            return state();

        for(auto &child : children) {
            if(LOGGING) {
                LOG_DEBUG("rss " + id() + " ticks ");
                LOG_DEBUG(child->id());
            }
            auto status = child->tick(tick_type);
            if(status.first == SUCCESS || status.first == FAILED) return status.first;
        }
        return RUNNING;
    }

    Node::NodeClass RunningSkippingSequence::node_class() const {
        return Node::RunningSkippingSequence;
    }


    const std::unordered_set<std::string>& ActiveNode::get_used_vars() const {
        return used_vars;
    }

    ActiveNode::ActiveNode(std::string id, Memory<double>& vars, std::string classifier,
            std::unordered_set<std::string> const &used_vars) : NoChildNode(id, vars, classifier), used_vars(used_vars) {}

    ActiveNode::~ActiveNode() {}

    const std::vector<Node*>& NoChildNode::get_children() const {
        return std::vector<Node*>();
    }

    int NoChildNode::children_size() const {
        return 0;
    }

    NoChildNode::NoChildNode(std::string id, bt::Memory<double> &vars, std::string classifier) : Node(id, vars, classifier) {}

    Condition::Condition(std::string id, Memory<double> &vars, const bt::Condition::ConditionaryFunction &function,
                         std::unordered_set<std::string> const &used_vars, std::string classifier):
            ActiveNode(id, vars, classifier, used_vars), function(function) {}

    Node::tick_return_type Condition::tick(TickType tick_type) {
        if(LOGGING)
            DEBUG_PR("TICK " + id());

        auto _old_state = state();
        vars.set(state_var(), evaluate(tick_type));
//        DEBUG_PR("tick " + id() + " " + STATE(state()) << " " << classifier());

        return return_tick(_old_state, state());
    }

    Node::State Condition::evaluate(TickType tick_type) {
        Node::State state(Node::RUNNING);
        try {
            state = function(vars);
        }
        catch (std::exception const& e) {
            DEBUG_PR("exception in evaluate "+ id());
        }
        if( LOGGING) {
            DEBUG_PR("evaluate " + id() + " " + STATE(state) << " " << classifier());
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

    Action::Action(std::string id, bt::Memory<double> &vars, const bt::Action::ActionaryFunction &function,
                   std::unordered_set<std::string> const &used_vars,
                   std::string classifier): ActiveNode(id, vars, classifier, used_vars), function(function) {}

    Node::tick_return_type Action::tick(TickType tick_type) {
        auto _old_state = state();
        vars.set(state_var(), evaluate(tick_type));
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
            LOG_DEBUG("ar_bt_test: action " + classifier() + " called");
        }
        return Node::SUCCESS;
    }

    Action& Action::operator=(const bt::Action &other) {
        function = other.function;
        return *this;
    }


    Latch::Latch(std::string id, bt::Memory<double> &vars, std::string latch_var_done, std::string classifier) : Sequential(id, vars, classifier) {
        if(latch_var_done == "")
            latch_done = "$_ld_" + id + "_ld_$";
        else
            latch_done = latch_var_done;
        vars.add_variable(latch_done);
        LOG_DEBUG("ar_bt_test : created latch over " + latch_done);
    }

    Node::tick_return_type Latch::tick(bt::Node::TickType tick_type) {
        if(LOGGING)
            DEBUG_PR("*" + id() + ' ' + STATE(state()) + ' ' + TICK_TYPE(tick_type) + " " << vars[latch_done])
        auto tick_children = call_table[state()][tick_type];
        auto _old_state = state();
        vars.set(state_var(), evaluate(tick_children));
        if(LOGGING)
            DEBUG_PR("!" + id() + ' ' + TICK_TYPE(tick_children) + ' ' + STATE(state()) + " " << vars[latch_done]);
        return return_tick(_old_state, state());
    }

    Node::State Latch::evaluate(bt::Node::TickType tick_type) {
        if(vars[latch_done] == 0){
            vars.set(state_var(), children[0]->tick(tick_type).first);
        }
        if(state() != RUNNING)
            vars.set(latch_done, true);
        return state();
    }

    Node::NodeClass Latch::node_class() const {
        return Node::Latch;
    }

    Node::NodeClass Action::node_class() const {
        return Node::Action;
    }

    Node::State SFRselector::evaluate(TickType tick_type) {
        if(tick_type == NO_TICK)
            return state();
        for(int i = 0; i < 3; i++) {
            if (children[i] == nullptr || children[i]->tick(tick_type).first == SUCCESS) return states[i];
        }
        return SUCCESS;
    }

    SFRselector::SFRselector(std::string id, Memory<double>& vars, std::string classifier) : Sequential(id, vars, classifier) {
        children = std::vector<Node*>(3, nullptr);
    }

    void SFRselector::set_child(Node *child, bt::Node::State state) {
        int place = 0;
        for(int i =0; i < Node::FINAL_STATE_ENTRY + 1; i++){
            if(states[i] == state) place = i;
        }
        children[ place ] = child;
    }

    SFRselector& SFRselector::operator=(const bt::SFRselector &other) {
        for(int i = 0; i < 3; i++) children[i] = other.children[i];
        return *this;
    }

    Node::NodeClass SFRselector::node_class() const {
        return Node::SFRselector;
    }

    Node::State bt::ConditionFabric::MergeTable[2][3][3] = {
            {
                    {Node::RUNNING, Node::RUNNING, Node::FAILED},
                    {Node::RUNNING, Node::SUCCESS, Node::FAILED},
                    {Node::FAILED, Node::FAILED, Node::FAILED}
            },
            {
                    {Node::RUNNING, Node::SUCCESS, Node::RUNNING},
                    {Node::SUCCESS, Node::SUCCESS, Node::SUCCESS},
                    {Node::RUNNING, Node::SUCCESS, Node::FAILED}
            }
    };

    bool bt::ConditionFabric::check(double lvalue, bt::ConditionFabric::CLogic op, double rvalue) {
        switch(op) {
            case CLogic::Less: return lvalue < rvalue;
            case CLogic::Greater: return lvalue > rvalue;
            case CLogic::Eq: return lvalue == rvalue;
            case CLogic::Neq: return lvalue != rvalue;
            case CLogic::Leq: return lvalue <= rvalue;
            case CLogic::Geq: return lvalue >= rvalue;
        }
        return false;
    }

    Node::State bt::ConditionFabric::check(Node::State lvalue, bt::ConditionFabric::MLogic op, Node::State rvalue) {
        return MergeTable[op][lvalue][rvalue];
    }
    bt::ConditionFabric::ConditionFabric(bt::Memory<double> &mem) : memory(mem), cond_id(0) {}

    std::vector<std::string> ConditionFabric::CLogic_op_list = {"<", ">", "==", "!=", "<=", ">="};

    std::string bt::ConditionFabric::op_to_string(bt::ConditionFabric::CLogic op) {
        return CLogic_op_list[size_t(op)];
    }

    std::string bt::ConditionFabric::op_to_string(bt::ConditionFabric::MLogic op) {
        return std::vector<std::string>{"&&", "||"}[size_t(op)];
    }

    int ConditionFabric::get_cond_id() {
        return cond_id++;
    }

    std::string ConditionFabric::get_cond_str_id(std::string id) {
        if (id == "")
            return "condition_" + std::to_string(get_cond_id());
        else
            return id;
    }

    Condition* ConditionFabric::add(bt::Condition * ptr) {
        store.push_back(ptr);
        return ptr;
    }

    bt::Condition* bt::ConditionFabric::create(std::string const &lvalue, bt::ConditionFabric::CLogic op, double rvalue,
            Node::State true_state, Node::State false_state,
                                              std::string id) {

        std::string classifier = lvalue + " " + op_to_string(op) + " " + std::to_string(rvalue);

        Condition::ConditionaryFunction f = [op, lvalue, rvalue, true_state, false_state](const Memory<double>& mem) -> bt::Node::State {
            return check(mem[lvalue], op, rvalue) ? true_state : false_state;
        };

        return add(new Condition(get_cond_str_id(id), memory, f, std::unordered_set<std::string>{lvalue}, classifier));
    }

    Condition* bt::ConditionFabric::create(std::string const &lvalue, bt::ConditionFabric::CLogic op,
                                          std::string const &rvalue, bt::Node::State true_state,
                                          bt::Node::State false_state, std::string id) {

        std::string classifier = lvalue + " " + op_to_string(op) + " " + rvalue;

        Condition::ConditionaryFunction f = [op, lvalue, rvalue, true_state, false_state](const Memory<double>& mem) -> bt::Node::State  {
            return check(mem[lvalue], op, mem[rvalue]) ? true_state : false_state;
        };

        return add(new Condition(get_cond_str_id(id), memory, f, std::unordered_set<std::string>{lvalue, rvalue}, classifier));
    }

    std::vector<std::string> split (std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }

        res.push_back (s.substr (pos_start));
        return res;
    }

    ConditionFabric::CLogic ConditionFabric::string_to_op(std::string const &str) {
        for(size_t i = 0; i < CLogic_op_list.size(); i++)
            if (CLogic_op_list[i] == str) return CLogic(i);


        return CLogic(0);
    }


    Condition* ConditionFabric::create(bt::Condition::ConditionaryFunction f,
                                       std::unordered_set<std::string> const &used_vars, std::string id) {
        return add(new Condition(get_cond_str_id(id), memory, f, used_vars, ""));
    }

    Condition * ConditionFabric::create_timer(Node::State after_state, std::string const &time_start, std::string const &timer_set,
                                              std::string const &time_to_wait, std::string time_var, std::string id) {
        return create ([time_start, timer_set, time_to_wait, time_var, after_state](Memory<double> const& m) -> bt::Node::State {
            return (m[timer_set] == 0 || m[time_var] - m[time_start] < m[time_to_wait]) ? Node::RUNNING : after_state;
        }, {time_start, timer_set, time_to_wait, time_var}, id);
    }
    Condition* ConditionFabric::create(std::string const &description, bt::Node::State true_state,
                                      bt::Node::State false_state, std::string id) {
        std::vector<std::string> seq = split(description, " ");
        if(seq.size() != 3) {
            return add(new Condition("", memory));
        }
        try {
            double rvalue = std::stod(seq[2]);
            return create(seq[0], string_to_op(seq[1]), rvalue, true_state, false_state, id);
        }
        catch(std::invalid_argument) {
            return create(seq[0], string_to_op(seq[1]), seq[2], true_state, false_state, id);
        }
    }

    Condition* bt::ConditionFabric::merge(const bt::Condition &left, bt::ConditionFabric::MLogic op,
                                         const bt::Condition &right, std::string id) {

        std::string classifier =  "(" + left.classifier() + ") " + op_to_string(op) + " (" + right.classifier() + ")";
        Condition::ConditionaryFunction f = [left, op, right](const Memory<double>& mem) -> bt::Node::State {
            return check(left.function(mem), op, right.function(mem));
        };
        auto used_vars = left.get_used_vars();
        for (auto const &v: right.get_used_vars()) {
            used_vars.insert(v);
        }
        return add(new Condition(get_cond_str_id(id), memory, f, used_vars, classifier));
    }

    Condition* ConditionFabric::merge(const bt::Condition &success, const bt::Condition &failure, std::string id) {


        std::string classifier = "[ S <- (" + success.classifier() + "),\n F <- (" + failure.classifier() + ") ]";

        Condition::ConditionaryFunction f = [success, failure](const Memory<double>& mem) -> bt::Node::State  {
            if(success.function(mem) == Node::SUCCESS) return Node::SUCCESS;
            if(failure.function(mem) == Node::SUCCESS) return Node::FAILED;
            return Node::RUNNING;
        };
        auto used_vars = success.get_used_vars();
        for (auto const &v: failure.get_used_vars()) {
            used_vars.insert(v);
        }
        return add(new Condition(get_cond_str_id(id), memory, f, used_vars, classifier));
    }

    ConditionFabric::~ConditionFabric() {
        for(auto p: store) delete p;
    }

    ActionFabric::ActionFabric(bt::Memory<double> &mem) : memory(mem), act_id(0) {}

    std::vector<std::string> ActionFabric::Arithmetic_op_list = {"+","-","*", "/"};

    ActionFabric::Arithmetic ActionFabric::string_to_op(std::string const &str) {
        for(size_t i = 0; i < Arithmetic_op_list.size(); i++)
            if (Arithmetic_op_list[i] == str) return Arithmetic(i);
        return Arithmetic::Add;
    }

    std::string ActionFabric::op_to_string(bt::ActionFabric::Arithmetic op) {
        return Arithmetic_op_list[int(op)];
    }

    int ActionFabric::get_act_id() {
        return act_id++;
    }

    std::string ActionFabric::get_act_str_id(std::string id) {
        if (id == "") {
            return "action_" + std::to_string(get_act_id());
        }
        else return id;
    }

    Action* ActionFabric::add(bt::Action *ptr) {
        store.push_back(ptr);
        return ptr;
    }

    Action* ActionFabric::create(bt::Action::ActionaryFunction f, std::string id) {
        return add(new Action(get_act_str_id(id), memory, f));
    }

    Action* ActionFabric::create(std::string const &lvalue, double rvalue, std::string id) {

        std::string classifier = lvalue + " = " + std::to_string(rvalue) + ";";
        Action::ActionaryFunction f = [lvalue, rvalue](Memory<double>& mem) {
            mem.set(lvalue, rvalue);
        };

        return add(new Action(get_act_str_id(id), memory, f, std::unordered_set<std::string>{lvalue}, classifier));
    }

    Action* ActionFabric::create(std::string const &lvalue, std::string const& rvalue, std::string id) {

        std::string classifier = lvalue + " = " + rvalue + ";";
        Action::ActionaryFunction f = [lvalue, rvalue](Memory<double>& mem) {
            mem.set(lvalue, mem[rvalue]);
        };

        return add(new Action(get_act_str_id(id), memory, f, std::unordered_set<std::string>{lvalue}, classifier));
    }



    Action* ActionFabric::create(std::string const &description, std::string id) {
        std::vector<std::string> seq = split(description, " ");
        if(seq.size() != 3 && seq.size() != 5) {
            return add(new Action("",memory));
        }
        if(seq.size() == 3) {
            try {
                double rvalue = std::stod(seq[2]);
                return create(seq[0], rvalue);
            }
            catch(std::invalid_argument) {
                return create(seq[0], seq[2], "");
            }
        }
        else {
            std::string classifier = description + ";";
            std::string const& lvalue = seq[0];
            std::string const& op = seq[3];
            std::string const& r2 = seq[4], r1 = seq[2];

            bool var1 = false, var2 = false;
            try {
                std::stod(r1);
                var1 = true;
            }
            catch(std::invalid_argument) {
            }

            try {
                std::stod(r2);
                var2 = true;
            }
            catch(std::invalid_argument) {
            }

            Action::ActionaryFunction f = [lvalue, op, r1, r2, var1, var2](Memory<double>& mem) {
                double res = 0;
                double rd1, rd2;
                if(!var1) rd1 = mem[r1];
                else rd1 = stod(r1);
                if(!var2) rd2 = mem[r2];
                else rd2 = stod(r2);

                LOG_DEBUG("ar_bt_test 11: " + lvalue + " = " + std::to_string(rd1) + " " + op + " " + std::to_string(rd2));
                switch(string_to_op(op)) {
                    case Arithmetic::Add: res = rd1 + rd2; break;
                    case Arithmetic::Sub: res = rd1 - rd2; break;
                    case Arithmetic::Mul: res = rd1 * rd2; break;
                    case Arithmetic::Div: res = rd1 / rd2; break;
                    default: res = rd1 + rd2;
                }
                LOG_DEBUG("ar_bt_test : " + lvalue + " = " + std::to_string(res));
                mem.set(lvalue, res);
            };
            std::unordered_set<std::string> used_vars;
            if (!var1) used_vars.insert(r1);
            if (!var2) used_vars.insert(r2);
            return add(new Action(get_act_str_id(id),memory, f, used_vars, classifier));
        }
    }

    Action* ActionFabric::unite(const bt::Action &first, const bt::Action &second, std::string id) {
        Action::ActionaryFunction f = [first, second](Memory<double>& mem) {
            first.function(mem);
            second.function(mem);
        };

        std::unordered_set<std::string> used_vars = first.get_used_vars();
        for(auto v: second.get_used_vars())
            used_vars.insert(v);

        std::string classifier = first.classifier() + "\n" + second.classifier();
        return add(new Action(get_act_str_id(id),memory, f, used_vars, classifier));
    }

    Action* ActionFabric::unite(std::initializer_list<Action> const& actions, std::string id) {
        return unite(std::vector<Action>(actions), id);
    }

    Action* ActionFabric::unite(const std::vector<bt::Action> &actions, std::string id) {
        Action::ActionaryFunction f = [actions](Memory<double>& mem) {
            for(auto const& a: actions)
                a.function(mem);
        };
        std::unordered_set<std::string> used_vars;
        for(auto const& a: actions) {
            for(auto const& v: a.get_used_vars()) {
                used_vars.insert(v);
            }
        }
        std::string classifier = "";
        for(auto const& a: actions)
            classifier += "\n" + a.classifier();
        classifier = classifier.substr(1);

        return add(new Action(get_act_str_id(id), memory, f, used_vars, classifier));
    }

    Action * ActionFabric::create_and_unite(std::initializer_list<std::string> descriptions, std::string id) {
        return create_and_unite(std::vector<std::string>(descriptions), id);
    }
    Action * ActionFabric::create_and_unite(std::vector<std::string> descriptions, std::string id) {
        std::vector<Action> actions;
        actions.reserve(descriptions.size());
        for(auto const& d: descriptions) {
            actions.push_back(*create(d));
        }
        return unite(actions, id);
    }

    ActionFabric::~ActionFabric() {
        for(auto p: store) delete p;
    }

    NodeFabric::NodeFabric(bt::Memory<double> &mem) : memory(mem), action(mem), condition(mem) {}

    std::string NodeFabric::get_node_str_id(std::string const &id, std::string const &type) {
        if(id == "")
            return type + "_" + std::to_string(node_id);
        else
            return id;
    }

    Sequence * NodeFabric::sequence(std::string id) {
        auto p = new Sequence(get_node_str_id(id, "sequence"), memory);
        seq_store.push_back(p);
        return p;
    }

    Selector * NodeFabric::selector(std::string id) {
        auto p =  new Selector(get_node_str_id(id, "selector"), memory);
        sel_store.push_back(p);
        return p;
    }

    RunningSkippingSequence * NodeFabric::rssequence(std::string id) {
        auto p = new RunningSkippingSequence(get_node_str_id(id, "rssequence"), memory);
        rss_store.push_back(p);
        return p;
    }

    Parallel * NodeFabric::parallel(std::string id) {
        auto p = new Parallel(get_node_str_id(id, "parallel"), memory);
        par_store.push_back(p);
        return p;
    }

    Latch * NodeFabric::latch(std::string var_name, std::string id) {
        auto p = new Latch(get_node_str_id(id, "latch"), memory, var_name);
        lat_store.push_back(p);
        return p;
    }

    NodeFabric::~NodeFabric() {
        for(auto p: par_store) delete p;
        for(auto p: sel_store) delete p;
        for(auto p: seq_store) delete p;
        for(auto p: rss_store) delete p;
        for(auto p: lat_store) delete p;
    }
};
