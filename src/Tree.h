//
// Created by safoex on 19.11.18.
//

#ifndef BEHAVIOR_TREE_TREE_H
#define BEHAVIOR_TREE_TREE_H

#include <unordered_map>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "Node.h"
#include <chrono>
#include <fstream>
#include "ExecutorBase.h"

namespace bt{
    class NodeInfo{
        friend class Tree;
        friend class TreeWithSync;
    protected:
        Node* node;
        int trace;
        std::unordered_set<std::string> children;
        std::string parent;
        std::vector<int> order;
    public:
        NodeInfo(NodeInfo const& other);
        explicit NodeInfo(Node* node = nullptr, std::string const& parent = "");
        std::string const& id() const;
        bool operator<(NodeInfo const& other) const;
        NodeInfo& operator=(NodeInfo const& other);
    };

    class Tree : public ExecutorBase {
    public:
        template<typename T> using dict = std::unordered_map<std::string, T>;
    protected:
        dict<NodeInfo> nodes;
        MemoryBase& memory;
        dict<std::unordered_set<std::string>> var_in_cond;
        std::string root_name;
        std::mutex lock;
        virtual void add_base_node(std::string const& parent_name, Node* node, Node::State where = Node::SUCCESS);
        int count_recursive;
        strset2 changed_conditions();
        strset2 saved_conditions;
        void clear_visited_from(std::string node = "");
        std::pair<std::string, std::string> get_node_name(Node* n, bool states);
    public:
        enum EXEC_STRATEGY {
            ASYNC,
            HALF_ASYNC
        };
        std::ofstream log_tree;
        Tree(MemoryBase& memory, std::string name = "__ROOT__");
        void add_node(std::string const& parent_name, Sequential* node, Node::State where = Node::SUCCESS);
        void add_node(std::string const& parent_name, Condition* node, Node::State where = Node::SUCCESS);
        void add_node(std::string const& parent_name, Action* node, Node::State where = Node::SUCCESS);
        MemoryBase& get_memory() override;
        sample callback(sample const& input) override;
        sample callback(sample const& input, bool need_to_lock);
        const std::string NEED_TO_LOCK_VAR = "__NEED_TO_LOCK__";
        std::string get_root_name();
        ~Tree() override;
        virtual sample start();
        sample init() override;
        sample& update_sample(sample& s) override;
        sample update_sample(sample const& s) const override;
        Node::State state();
        std::string tree_description(bool states = false);
        std::string dot_tree_description(bool states = false);
        bool any_condition_changed();
        explicit operator std::string();
        void set_strategy(EXEC_STRATEGY es);
    protected:
        std::map<NodeInfo, Node::TickType> nodes_to_tick;
        void propogate_once();
        void propagate_once_top_down();
        EXEC_STRATEGY strategy;
    };

    std::string print_sample(sample input);
};

#endif //BEHAVIOR_TREE_TREE_H
