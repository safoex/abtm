//
// Created by safoex on 19.11.18.
//

#ifndef BEHAVIOR_TREE_NODE_H
#define BEHAVIOR_TREE_NODE_H


#include <string>
#include "Memory.h"
#include <list>
#include <vector>
#include <list>
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
            UNDEFINED,
            FINAL_STATE_ENTRY = UNDEFINED
        };
        enum TickType {
            TOPDOWN_FALL,
            TOPDOWN_RISE,
            BOTTOMUP_FALL,
            BOTTOMUP_RISE,
            DEACTIVATION_RUN,
            DEACTIVATION_AFTER,
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
        static Node::TickType return_tick_table[FINAL_STATE_ENTRY+1][FINAL_STATE_ENTRY+1];
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
        virtual const std::list<Node*>& get_children() const = 0;
        virtual int children_size() const = 0;
        virtual NodeClass node_class() const = 0;
    };


    class Sequential : public Node
    {
        const size_t children_limit = -1;
        static TickType call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY];
    protected:
        std::list<Node*> children;
    public:
        Sequential(std::string id, Memory<double>& vars, std::string classifier);
        void add_child(Node* child);
        void insert_child(Node* child, std::string const& child_name, bool after);
        void remove_child(std::string const& child_name);
        tick_return_type tick(TickType tick_type) override;
        Sequential& operator=(const Sequential& other);
        ~Sequential() override;
        const std::list<Node*>& get_children() const override;
        int children_size() const override;
        virtual State evaluate(TickType tick_type);
        virtual State return_state() = 0;
        virtual Node::TickType get_call_table(Node::State s, Node::TickType t);
        virtual void start_deactivation(TickType tick_type, Node::tick_return_type tick_return);
    };

    class Selector : public Sequential {
        const int children_limit = -1;
    public:
        explicit Selector(std::string id, Memory<double>& vars, std::string classifier = "");
        NodeClass node_class() const override;
        inline State return_state() override;
    };

    class Sequence : public Sequential {
        const int children_limit = -1;
    public:
        explicit Sequence(std::string id, Memory<double>& vars, std::string classifier = "");
        NodeClass node_class() const override;
        inline State return_state() override;
    };

    class RunningSkippingSequence : public Sequential {
        const int children_limit = -1;
        static TickType call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY];
        virtual Node::TickType get_call_table(Node::State s, Node::TickType t) override;
    public:
        explicit RunningSkippingSequence(std::string id, Memory<double>& vars, std::string classifier = "");
        NodeClass node_class() const override;
        inline State return_state() override;
        void start_deactivation(TickType tick_type, Node::tick_return_type tick_return) override;
    };


    class Parallel : public Sequential {
        const int children_limit = -1;
        static TickType call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY];
    public:
        explicit Parallel(std::string id, Memory<double>& vars, std::string classifier = "");
        State evaluate(TickType tick_type) override;
        NodeClass node_class() const override;
        inline State return_state() override;
        Node::TickType get_call_table(Node::State s, Node::TickType t) override;
    };



    class NoChildNode : public Node {
    public:
        NoChildNode(std::string id, Memory<double>& vars,  std::string classifier);
        const std::list<Node*>& get_children() const override;
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
        static TickType call_table[FINAL_STATE_ENTRY+1][FINAL_TICK_TYPE_ENTRY];
    public:
        explicit Latch(std::string id, Memory<double>& vars, std::string latch_var_done = "", std::string classifier = "");
        tick_return_type tick(TickType tick_type) override;
        State evaluate(TickType tick_type) override;
        NodeClass node_class() const override;
        inline State return_state() override;
        virtual Node::TickType get_call_table(Node::State s, Node::TickType t) override;
    };


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

};




#endif //BEHAVIOR_TREE_NODE_H
