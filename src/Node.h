//
// Created by safoex on 19.11.18.
//

#ifndef BEHAVIOR_TREE_NODE_H
#define BEHAVIOR_TREE_NODE_H


#include <string>
#include "Memory.h"
#include <list>
#include <vector>
#include <functional>
#include <iostream>

#define DEBUG true
#define DEBUG_PR(A) if(DEBUG) std::cout << A << std::endl;

namespace bt {

    std::string STATE(int x);
    std::string TICK_TYPE(int x);
    std::string NODE_CLASS(int x);

    class Node {
    public:
        enum State {
            RUNNING,
            SUCCESS,
            FAILED,
            FINAL_STATE_ENTRY = FAILED
        };
        enum TickType {
            TOPDOWN_FALL,
            TOPDOWN_RISE,
            BOTTOMUP_FALL,
            BOTTOMUP_RISE,
            NO_TICK,
            FINAL_TICK_TYPE_ENTRY = NO_TICK
        };
        typedef std::pair<Node::State, Node::TickType> tick_return_type;
        enum NodeClass {
            Sequence,
            Selector,
            Parallel,
            RunningSkippingSequence,
            SFRselector,
            Latch,
            Action,
            Condition,
            FINAL_NODE_CLASS_ENTRY = Condition
        };
        static Node::TickType return_tick_table[3][3];
    protected:
        std::string _id, _classifier;
        Memory<double>& vars;
        tick_return_type return_tick(Node::State before, Node::State after) const;
    public:
        explicit Node(std::string id, Memory<double >& vars, std::string classifier = "");
        virtual ~Node();
        virtual tick_return_type tick(TickType tick_type = BOTTOMUP_RISE) = 0;
        virtual State evaluate(TickType tick_type) = 0;
        const std::string& id() const;
        const std::string& classifier() const;
        std::string state_var() const;
        Node::State state() const;
        bool hide_further; // visualization specific
        virtual const std::vector<Node*>& get_children() const = 0;
        virtual int children_size() const = 0;
        virtual NodeClass node_class() const = 0;
    };


    class Sequential : public Node
    {
        const size_t children_limit = -1;
        static TickType call_table[3][4];
    protected:
        std::vector<Node*> children;
    public:
        Sequential(std::string id, Memory<double>& vars, std::string classifier);
        void add_child(Node* child);
        tick_return_type tick(TickType tick_type) override;
        Sequential& operator=(const Sequential& other);
        ~Sequential() override;
        const std::vector<Node*>& get_children() const override;
        int children_size() const override;
    };

    class Selector : public Sequential {
        const int children_limit = -1;
    public:
        explicit Selector(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type) override;
        NodeClass node_class() const override;
    };

    class Sequence : public Sequential {
        const int children_limit = -1;
    public:
        explicit Sequence(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type) override;
        NodeClass node_class() const override;
    };

    class Parallel : public Sequential {
        const int children_limit = -1;
    public:
        explicit Parallel(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type) override;
        NodeClass node_class() const override;
    };

    class RunningSkippingSequence : public Sequential {
        const int children_limit = -1;
    public:
        explicit RunningSkippingSequence(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type = TickType::BOTTOMUP_RISE) override;
        NodeClass node_class() const override;
    };


    class NoChildNode : public Node {
    public:
        NoChildNode(std::string id, Memory<double>& vars,  std::string classifier);
        const std::vector<Node*>& get_children() const override;
        int children_size() const override;
    };


    class ActiveNode : public NoChildNode {
    protected:
        std::unordered_set<std::string> used_vars;
    public:
        ActiveNode(std::string id, Memory<double>& vars,  std::string classifier,
                std::unordered_set<std::string> const &used_vars);
        const std::unordered_set<std::string>& get_used_vars() const;
        ~ActiveNode() override;
    };

    class Latch : public Sequential {
    protected:
        const int children_limit = 1;
        std::string latch_done;
        static TickType call_table[3][4];
    public:
        explicit Latch(std::string id, Memory<double>& vars, std::string latch_var_done = "", std::string classifier = "");
        tick_return_type tick(TickType tick_type) override;
        State evaluate(TickType tick_type = TickType::BOTTOMUP_RISE) override;
        NodeClass node_class() const override;
    };


    class ConditionFabric;

    class Condition : public ActiveNode {
        friend class ConditionFabric;
    public:
        typedef std::function<State(const Memory<double>&)> ConditionaryFunction;
    protected:
        ConditionaryFunction function;
    public:
        State evaluate(TickType tick_type) override;
        tick_return_type tick(TickType tick_type) override;
        explicit Condition(std::string id,
                           Memory<double> &vars,
                           const ConditionaryFunction &function = [](
                                   const Memory<double> &a) -> State { return State::SUCCESS; },
                           std::unordered_set<std::string> const &used_vars = std::unordered_set<std::string>(),
                           std::string classifier = "");
        Condition& operator=(const Condition& other);
        NodeClass node_class() const override;
    };

    class ActionFabric;

    class Action : public ActiveNode {
        friend class ActionFabric;
    public:
        typedef std::function<void(Memory<double>&)> ActionaryFunction;
    protected:
        ActionaryFunction function;
    public:
        Action(std::string id,
               Memory<double> &vars,
               const ActionaryFunction &function = [](Memory<double> &a) -> void {},
               std::unordered_set<std::string> const &used_vars = std::unordered_set<std::string>(),
               std::string classifier = "");
        tick_return_type tick(TickType tick_type) override;
        State evaluate(TickType tick_type) override;
        Action& operator=(const Action& other);
        NodeClass node_class() const override;
    };

    //TODO: template <int N, vars..> VariableSelector : public Node {};
    //TODO: typedef VariableSelector<3, SUCCESS, FAILED, RUNNING> SFRselector;


    class SFRselector : public Sequential {
    protected:
        const int children_limit = 3;
    public:
        const std::vector<State> states{SUCCESS, FAILED, RUNNING};
        explicit SFRselector(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type) override;
        void set_child(Node *child, Node::State state = State::SUCCESS);
        SFRselector& operator=(const SFRselector& other);
        NodeClass node_class() const override;
    };


    class ConditionFabric {
        Memory<double>& memory;
        std::list<Condition*> store;
        Condition* add(Condition* ptr);
    public:
        enum CLogic {
            Less,
            Greater,
            Eq,
            Neq,
            Leq,
            Geq,
        };
        enum MLogic {
            And,
            Or
        };
        static Node::State MergeTable[2][3][3];
        static std::vector<std::string> CLogic_op_list;
        int cond_id;
        int get_cond_id();
        std::string get_cond_str_id(std::string id = "");
        static std::string op_to_string(CLogic op);
        static std::string op_to_string(MLogic op);
        static CLogic string_to_op(std::string const& str);
        static bool check(double lvalue, CLogic op, double rvalue);
        static Node::State check(Node::State lvalue, MLogic op, Node::State rvalue);
        ConditionFabric(Memory<double>& mem);
        Condition* create_timer(Node::State after_state, std::string const& time_start, std::string const& timer_set, std::string const& time_to_wait,
                std::string time_var = "time", std::string id = "");
        Condition* create(Condition::ConditionaryFunction f, std::unordered_set<std::string> const& used_vars, std::string id = "");
        Condition* create(std::string const& description, Node::State true_state = Node::SUCCESS,
                         Node::State false_state = Node::RUNNING, std::string id = "");
        Condition* create(std::string const& lvalue, CLogic op, std::string const& rvalue, Node::State true_state = Node::SUCCESS,
                Node::State false_state = Node::RUNNING, std::string id = "");
        Condition* create(std::string const& lvalue, CLogic op, double rvalue, Node::State true_state = Node::SUCCESS,
                         Node::State false_state = Node::RUNNING, std::string id = "");
        Condition* merge(Condition const& success, Condition const& failure, std::string id = "");
        Condition* merge(Condition const& left, MLogic op, Condition const& right, std::string id = "");
        ~ConditionFabric();
    };

    class ActionFabric {
        Memory<double>& memory;
        std::list<Action*> store;
        Action* add(Action* ptr);
    public:
        int act_id;
        int get_act_id();
        std::string get_act_str_id(std::string id);
        enum Arithmetic {
            Add,
            Sub,
            Mul,
            Div
        };
        static std::vector<std::string> Arithmetic_op_list;
        static std::string op_to_string(Arithmetic op);
        static Arithmetic string_to_op(std::string const& str);
        ActionFabric(Memory<double>& mem);
        Action* create(Action::ActionaryFunction f, std::string id = "");
        Action* create(std::string const& lvalue, double rvalue, std::string id = "");
        Action* create(std::string const& lvalue, std::string const& rvalue, std::string id);
        Action* create(std::string const& description, std::string id = "");
        Action* unite(Action const& first, Action const& second, std::string id="");
        Action* unite(std::vector<Action> const& actions, std::string id="");
        Action* create_and_unite(std::initializer_list<std::string> descriptions, std::string id="");
        Action* create_and_unite(std::vector<std::string> descriptions, std::string id="");
        Action* unite(std::initializer_list<Action> const& actions, std::string id="");
        ~ActionFabric();
    };


    class NodeFabric {
        Memory<double>& memory;
        std::list<Sequence*> seq_store;
        std::list<Parallel*> par_store;
        std::list<Selector*> sel_store;
        std::list<RunningSkippingSequence*> rss_store;
        std::list<Latch*> lat_store;
    public:
        ActionFabric action;
        ConditionFabric condition;
    public:
        int node_id;
        std::string get_node_str_id(std::string const& id, std::string const& type);
        NodeFabric(Memory<double>& mem);
        Sequence* sequence(std::string id = "");
        Parallel* parallel(std::string id = "");
        RunningSkippingSequence* rssequence(std::string id = "");
        Latch* latch(std::string var_name = "", std::string id = "");
        Selector* selector(std::string id = "");
        ~NodeFabric();
    };

};




#endif //BEHAVIOR_TREE_NODE_H
