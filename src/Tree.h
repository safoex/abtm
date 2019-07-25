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
        Memory<double> memory;
        dict<std::unordered_set<std::string>> var_in_cond;
        std::string root_name;
        std::mutex lock;
        virtual void add_base_node(std::string const& parent_name, Node* node, Node::State where = Node::SUCCESS);
        int count_recursive;
        strset2 changed_conditions();
        strset2 saved_conditions;
    public:
        virtual double operator[](std::string const& key) const;
        std::ofstream log_tree;
        Tree(std::string name = "__ROOT__");
        void add_node(std::string const& parent_name, Sequential* node, Node::State where = Node::SUCCESS);
        void add_node(std::string const& parent_name, Condition* node, Node::State where = Node::SUCCESS);
        void add_node(std::string const& parent_name, Action* node, Node::State where = Node::SUCCESS);
        Memory<double>& get_memory();
        virtual dict<double> callback(dict<double> sample, bool need_to_lock = true);\
        const std::string NEED_TO_LOCK_VAR = "__NEED_TO_LOCK__";
        sample callback(sample const& input) override;
        std::string get_root_name();
        ~Tree() override;
        virtual bt::Tree::dict<double> start();
        sample init() override;
        sample& update_sample(sample& s) override;
        sample update_sample(sample const& s) const override;
        Node::State state();
        std::string tree_description(bool states = false);
        std::string dot_tree_description(bool states = false);
        bool any_condition_changed();
        explicit operator std::string();
        dict<double> filter(dict<double> sample) const;
    protected:
        std::map<NodeInfo, Node::TickType> nodes_to_tick;
        void propogate_once();
    };

    std::string print_sample(Tree::dict<double> sample);
};

#endif //BEHAVIOR_TREE_TREE_H
