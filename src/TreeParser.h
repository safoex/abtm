//
// Created by safoex on 04.03.19.
//

#ifndef AR_BUILD_ROOT_TREEPARSER_H
#define AR_BUILD_ROOT_TREEPARSER_H

#include <string>
#include <Tree.h>
#include <yaml-cpp/yaml.h>

namespace bt {

    class RValueParser {
        typedef std::function<double(const Memory<double>&)> RValueFunction;
    protected:
        Memory<double> &m;

    public:
        RValueParser(Memory<double> &memory);

        Action::ActionaryFunction get_actionary_function(std::string const& lvalue, std::string const& rvalue) const;
        Condition::ConditionaryFunction get_conditionary_function(std::string const& success = "default",
                std::string const& failed = "__never", std::string const& running = "__never") const;

        RValueFunction get_rvalue_function(std::string const& rvalue) const;
    };


#define FOR_EACH_TYPE(MACRO) \
        MACRO(Condition) \
        MACRO(Action) \
        MACRO(Sequence) \
        MACRO(Parallel) \
        MACRO(Selector) \
        MACRO(RunningSkippingSequence)\
        MACRO(Latch)

#define DECL_DICT(T) Tree::dict<T*> T##s;

#define DECL_ADD(T) void add(std::string const& key, T* n);

    class Store {
        FOR_EACH_TYPE(DECL_DICT)
    public:
        Store();
        ~Store();
        FOR_EACH_TYPE(DECL_ADD)
    friend class TreeParser;
    };

    class TreeParser {
    protected:
        Store store;
        std::string file_;
        Tree* tree;
        RValueParser rvp;
        size_t nit;
        Tree::dict<std::vector<std::string>> graph;
        std::unordered_set<std::string> load_nodes(const YAML::Node &ye);
        std::unordered_set<std::string> load_node(std::string const &id, const YAML::Node &node);
        void load_field(const YAML::Node &ye, std::string const &key, std::string &yo) const;
        void load_variables(const YAML::Node &ye);
        void load_topics(const YAML::Node &ye);
        void load_commands(const YAML::Node &ye);
        void load_servers(const YAML::Node &ye);
        void init_vars(const YAML::Node &ye);
        void build_graph();
        std::string apply_sample(const YAML::Node &ye);
        void load_parameters(const YAML::Node &ye);
        std::unordered_set<std::string> get_extra_vars(std::unordered_set<std::string> const& vars);
    public:

        Tree::dict<int> parameters;
        TreeParser(Tree* tree, std::string const& file);
        void load();
        std::string apply_samples(std::string const& file);
        std::string get_graph_viz_description() const;

    };
};

#endif //AR_BUILD_ROOT_TREEPARSER_H
