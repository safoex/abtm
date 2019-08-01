//
// Created by safoex on 19.11.18.
//

#include "Tree.h"
#include "Memory.cpp"
#include <map>
#include <cassert>
#include <iostream>
#include <stack>

namespace bt {

    NodeInfo::NodeInfo(bt::Node *node, const std::string &parent) : trace(0), node(node), parent(parent) {}

    NodeInfo::NodeInfo(const bt::NodeInfo &other) {
        *this = other;
    }

    std::string const& NodeInfo::id() const {
        return node->id();
    }

    bool NodeInfo::operator<(const bt::NodeInfo &other) const {
        for(size_t i = 0; i < std::min(order.size(), other.order.size()); i++) {
            if( order[i] != other.order[i] )
                return order[i] < other.order[i]; // left, top..
        }
        return order.size() > other.order.size(); // direct child first!!!
    }

    NodeInfo& NodeInfo::operator=(const bt::NodeInfo &other) {
        node = other.node;
        trace = other.trace;
        children = other.children;
        parent = other.parent;
        order = other.order;
        return *this;
    }

    Tree::Tree(std::string name) : ExecutorBase(), root_name(name), log_tree("log_tree_" + name + ".txt"),
    count_recursive(0) {
        auto root = new Sequence(get_root_name(), get_memory());
        NodeInfo ri(root, "");
        ri.order.push_back(0);
        nodes[get_root_name()] = ri;
    }

    Tree::~Tree() {
//        for(const auto& p: nodes) {
//            delete p.second.node;
//        }
    }

    Memory<double>& Tree::get_memory() {
        return memory;
    }

    bool Tree::any_condition_changed() {
        //DEBUG_PR("entered any_cond");
        auto var_changed = memory.get_changes(Memory<double>::INNER);
        auto const& out_changes = memory.get_changes(Memory<double>::OUTPUT);
        var_changed.insert(out_changes.begin(), out_changes.end());

        strset2 cond_candidates;
        for(auto const& v: var_changed) {
            cond_candidates.insert(var_in_cond[v.first].begin(), var_in_cond[v.first].end());
        }
        int i = 0;
        for(auto const& c: cond_candidates) {
            i++;
            auto state_after = nodes[c].node->evaluate(Node::TOPDOWN_FALL);
//            DEBUG_PR("any cond changed " << i << ": " << nodes[c].node->id() + " " + STATE(nodes[c].node->state()) + " " + STATE(state_after));
            if(state_after != nodes[c].node->state())
                return true;
        }
//        return !saved_conditions.empty();
        return false;
    }

    strset2 Tree::changed_conditions() {
        auto var_changed = memory.get_changes(Memory<double>::INNER);
        auto const& out_changes = memory.get_changes(Memory<double>::OUTPUT);
        var_changed.insert(out_changes.begin(), out_changes.end());
        strset2 cond_candidates, changed_conditions;
        for(auto const& v: var_changed) {
            auto const& vic = var_in_cond[v.first];
            cond_candidates.insert(vic.begin(), vic.end());
        }
        for(auto const& c: cond_candidates) {
            auto state_after = nodes[c].node->evaluate(Node::TOPDOWN_FALL);
            if(state_after != nodes[c].node->state())
                changed_conditions.insert(c);
        }
        return changed_conditions;
    }

    void Tree::propogate_once() {
        //DEBUG_PR("PROP ONCE");
        // first take changes of any variables and change conditions

        auto var_changed = memory.get_changes(Memory<double>::INNER);
        auto out_changes = memory.get_changes(Memory<double>::OUTPUT);
        var_changed.insert(out_changes.begin(), out_changes.end());
        memory.clear_changes(Memory<double>::INNER);
        strset2 cond_candidates;
        for(auto const& v: var_changed) {
            cond_candidates.insert(var_in_cond[v.first].begin(), var_in_cond[v.first].end());
        }
        if(!saved_conditions.empty()) {
            for (auto const &sc: saved_conditions)
                cond_candidates.insert(sc);
            saved_conditions.clear();
            DEBUG_PR("USED SAVED CONDITIONS");
            }
//        DEBUG_PR("condition candidates size: " << cond_candidates.size());
        for(auto const& c: cond_candidates) {
            auto p = nodes[c].node->tick();
            if(p.second == Node::TOPDOWN_RISE || p.second == Node::BOTTOMUP_RISE)
                if(nodes_to_tick.count(nodes[nodes[c].parent]))
                    nodes_to_tick[nodes[nodes[c].parent]] = std::min(p.second, nodes_to_tick[nodes[nodes[c].parent]]);
                else
                    nodes_to_tick[nodes[nodes[c].parent]] = p.second;
        }


        if(!nodes_to_tick.empty()) {
            // pick lowest by order control node for execution
//            LOG_DEBUG("prop once, node_to_tick: " << nodes_to_tick.size());
            auto p = *nodes_to_tick.begin();
            auto const &node = p.first;
            Node::TickType t = p.second;
            nodes_to_tick.erase(node);

            auto tick_result = node.node->tick(t);
            log_tree << "ticked " + node.id() << " " << STATE(tick_result.first) << std::endl;
            if(tick_result.second == Node::TOPDOWN_RISE || tick_result.second == Node::BOTTOMUP_RISE) {
                if(!node.parent.empty())
                    nodes_to_tick[nodes[node.parent]] = tick_result.second;
            }


        }
//        DEBUG_PR("EXITED PROP ONCE");
    }

    bt::Tree::dict<double>  Tree::callback(bt::Tree::dict<double> sample, bool need_to_lock) {
        if(need_to_lock) lock.lock();
        memory.set(sample);
        while(any_condition_changed() || !nodes_to_tick.empty()) {
            propogate_once();
        }
        Tree::dict<double> output = memory.get_changes(Memory<double>::OUTPUT);
        memory.clear_changes(Memory<double>::OUTPUT);
        memory.clear_changes(Memory<double>::INNER);
        if(!output.empty())// || sample.count("connection_lost"))
//            DEBUG_PR("cb output: " << print_sample(output));
            DEBUG_PR("--------------------------");
        if(need_to_lock)
            lock.unlock();
        return output;
    }

    std::string Tree::get_root_name()  {
        return root_name;
    }

    void Tree::add_base_node(std::string const &parent_name, bt::Node *node, Node::State where) {
        NodeInfo ni(node, parent_name);
        auto &pa = nodes[parent_name];

        auto paseq_p = dynamic_cast<Sequential*>(pa.node);
        paseq_p->add_child(node);

        ni.order = nodes[parent_name].order;
        auto children = pa.node->get_children();
        size_t i = 0;
        for(auto it = children.begin(); it != children.end(); it++) {
            if (*it == node) {
                ni.order.push_back(i);
                break;
            }
            i++;
        }
        nodes[ni.id()] = ni;
    }

    void Tree::add_node(std::string const &parent_name, bt::Action *node, bt::Node::State where) {
        add_base_node(parent_name, node, where);
        for(auto const& v: node->get_used_vars()) {
            memory.add_variable(v, Memory<double>::Scope::INNER);
        }
    }

    void Tree::add_node(std::string const &parent_name, bt::Condition *node, bt::Node::State where) {
        add_base_node(parent_name, node, where);
        for(auto const& v: node->get_used_vars()) {
            memory.add_variable(v, Memory<double>::Scope::INNER);
            var_in_cond[v].insert(node->id());
        }
    }

    void Tree::add_node(std::string const &parent_name, bt::Sequential *node, bt::Node::State where) {
        add_base_node(parent_name, node, where);
    }

    Node::State Tree::state() {
        return nodes[get_root_name()].node->state();
    }

    bt::Tree::dict<double> Tree::start() {
        nodes[get_root_name()].node->tick(Node::TOPDOWN_FALL);
        dict <double> empty;
        return callback(empty);
    }


    std::string Tree::tree_description(bool states) {
        Node* node = nodes[get_root_name()].node;
        std::stack<std::pair<Node*, std::string>> dfs;
        std::string result;
        dfs.push(std::make_pair(node, ""));
        while(!dfs.empty()) {
            auto p = dfs.top();
            dfs.pop();
            std::string desc;
            if(states)
                desc = STATE(p.first->state());
            else
                desc = p.first->classifier();
            result += p.second  +  NODE_CLASS((int)p.first->node_class()) + " " + p.first->id() + " " + desc + "\n";
            //DEBUG_PR(result);
            std::string delim = p.second + "  ";
            if(p.first->children_size() > 0) {
                const auto &children = p.first->get_children();
                for (auto cit = children.rbegin(); cit != children.rend(); cit++) {
                    dfs.push(std::make_pair(*cit, delim));
                }
            }
        }
        return result;
    }

    std::pair<std::string, std::string> get_node_name(Node* n, bool states) {
        std::unordered_map<int, std::pair<std::string, std::string>> colors{
                {Node::Sequence,{"white","->"}},
                {Node::Selector,{"white","?"}},
                {Node::Parallel,{"white","="}},
                {Node::RunningSkippingSequence, {"white","=>"}},
                {Node::SFRselector, {"white","???"}},
                {Node::Latch, {"yellow","^"}},
                {Node::Action,{"green",""}},
                {Node::Condition,{"orange",""}}
        };
        std::string desc;
        if(states)
            desc = STATE(n->state());
        else
            desc = n->classifier();

        //define node

        std::string gv_node_name;
        gv_node_name += "\"";
        auto const& p2 = colors[n->node_class()];
        std::string color = p2.first, hat = p2.second;
        if(n->hide_further) color = "\"#16abc9\"";
        if(hat != "")
            gv_node_name += hat + "\n";
        gv_node_name += "" + n->id() + "";
        if(desc != "")
            gv_node_name += "\n" + desc;
        gv_node_name += "\"";
        std::string fontcolor = "black";
        if(n->node_class() != Node::Action && n->node_class() != Node::Condition && desc == "RUNNING")
            fontcolor = "red";
        return {"\"" + n->id() + "\"", "\""+n->id()+"\"" + "[label=" +  gv_node_name + ", color=" + color
                    + ", fontcolor=" + fontcolor + "];\n"};

    }

    std::string Tree::dot_tree_description(bool states) {
        std::string result;
        result += "digraph g {\n";
        result += "node [shape=rectangle, style=filled, color=white];\n";


        dict<std::pair<std::string, std::string>> gv_names;

        for(auto const& p:nodes){


            auto const& n = p.second.node;
            //DEBUG_PR(result);

            gv_names[n->id()] = get_node_name(n, states);
            //result += n_r.second;
        }

        std::stack<Node*> dfs;
        Node* node = nodes[get_root_name()].node;
        dfs.push(node);
        std::string constraints;
        while(!dfs.empty()) {
            auto n = dfs.top();
            dfs.pop();
            result += gv_names[n->id()].second;
            //result += p.second  +  NODE_CLASS((int)p.first->node_class()) + " " + p.first->id() + " " + desc + "\n";
            //DEBUG_PR(result);
            std::string constraint = "{ rank = same;\n";
            if(n->children_size() > 0 && !n->hide_further) {
                const auto &children = n->get_children();
                for (auto cit = children.rbegin(); cit != children.rend(); cit++) {
                    dfs.push(*cit);
                }
                for( auto cit = children.begin(); cit != children.end(); cit++) {
                    result += gv_names[n->id()].first + " -> " + gv_names[(*cit)->id()].first + ";\n";
                    if(cit != children.begin()) {
                        constraint += " -> ";
                    }
                    constraint +=  gv_names[(*cit)->id()].first;
                }
                constraint+="[style=invis];\n}\n";
                if(n->children_size() > 1) constraints += constraint;
            }


        }


        result += constraints;
        result += "\n}";
        return result;
    }

    Tree::operator std::string() {
        return tree_description();
    }

    Tree::dict<double> Tree::filter(Tree::dict<double> sample) const {
        auto res = sample;
        for(auto kv : sample)
            if(memory.have(kv.first))
                res.insert(kv);
        return res;
    }

    std::string print_sample(Tree::dict<double>  sample) {
        std::string print_out("");
        if(!sample.empty())
        for(auto v: sample)
            print_out += v.first + ": " + std::to_string(v.second)+"; ";
        return print_out;
    }


    double Tree::operator[](std::string const &key) const {
        if(memory.have(key))
            return memory[key];
        else return 0;
    }


    sample Tree::callback(const bt::sample &input) {
        dictOf<double> s;
        for(auto const& ikv: input)
            s[ikv.first] = std::any_cast<double>(ikv.second);

        s = callback(s, input.count(NEED_TO_LOCK_VAR));

        sample output;
        for(auto const& okv: s) {
            output[okv.first] = okv.second;
        }
        return output;
    }

    sample Tree::init() {
        auto s = start();

        sample output;
        for(auto const& okv: s)
            output[okv.first] = okv.second;

        return output;
    }

    sample& Tree::update_sample(bt::sample &s) {
        for(auto& kv: s)
            s[kv.first] = memory[kv.first];
        return s;
    }

    sample Tree::update_sample(bt::sample const& s) const {
        auto r(s);
        for(auto& kv: s)
            r[kv.first] = memory[kv.first];
        return r;
    }

};
