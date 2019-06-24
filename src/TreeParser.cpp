//
// Created by safoex on 04.03.19.
//

#include "TreeParser.h"
#include "Memory.cpp"
#include <queue>

using namespace std;
namespace bt {
    RValueParser::RValueParser(bt::Memory<double> &memory) : m(memory) {}

    Action::ActionaryFunction RValueParser::get_actionary_function(std::string const &lvalue,
                                                                   std::string const &rvalue) const {
        return [lvalue, rvalue, this](Memory<double> &m) {
            double rv = this->get_rvalue_function(rvalue)(m);
            LOG_DEBUG("set "+ lvalue + " to " + std::to_string(rv));
            m.set(lvalue, rv);
        };
    }

    Condition::ConditionaryFunction RValueParser::get_conditionary_function(const std::string &success,
                                                                            const std::string &failed,
                                                                            const std::string &running) const {
        std::vector<std::string> funcs{failed, success, running};
        std::vector<Node::State> states{Node::FAILED, Node::SUCCESS, Node::RUNNING};
        return [funcs, states, this](const Memory<double> &m) -> Node::State  {
            for(int i = 0; i < 3; i++) {
                auto const &f = funcs[i];
                if (f != "" && f != "default") {
                    if(this->get_rvalue_function(f)(m)) {
                        return states[i];
                    }
                }
            }

            for(int i = 0; i < 3; i++) {
                if(funcs[i] == "default")
                    return states[i];
            }

            return Node::RUNNING;
        };
    }

    Store::Store() {}

#define DEL_NODES(T) for(auto const& n: T##s) delete n.second;

    Store::~Store() {
        FOR_EACH_TYPE(DEL_NODES)
    }

#define ADD_FUNC(T) void Store::add(std::string const& key, T* n) {T##s.insert({key, n});}

    FOR_EACH_TYPE(ADD_FUNC)


    bool delim (char c) {
        return c == ' ';
    }

    bool is_op (char c) {
        static std::set<char> ops{'+','-','*','/','>','<','=','!','&','|', '!'};
        return ops.count(c);
    }

    int priority (char op) {
        if (op < 0)
            return 6; // op == -'+' || op == -'-'
        return
                op == '&' || op == '|' ? 1 :
                op == '<' || op == '>' || op == '=' || op == '!' ? 2 :
                op == '+' || op == '-' ? 3 :
                op == '*' || op == '/' || op == '%' ? 4 :
                -1;
    }

    void process_op (vector<double> & st, char op) {
        if (op < 0) {
            double l = st.back();  st.pop_back();
            switch (-op) {
                case '+':  st.push_back (l);  break;
                case '-':  st.push_back (-l);  break;
            }
        }
        else {
            double r = st.back();  st.pop_back();
            double l = st.back();  st.pop_back();
            switch (op) {
                case '+':   st.push_back (l + r);  break;
                case '-':   st.push_back (l - r);  break;
                case '*':   st.push_back (l * r);  break;
                case '/':   st.push_back (l / r);  break;
                case '>':   st.push_back (l > r);  break;
                //case '>=':  st.push_back (l >= r);  break;
                case '<':   st.push_back (l < r);  break;
                //case '<=':  st.push_back (l <= r);  break;
                case '=':   st.push_back (l == r);  break;
                case '!':   st.push_back (l != r); break;
                case '&':   st.push_back (l && r); break;
                case '|':   st.push_back (l || r); break;
            }
        }
    }

    bool isunary(char op) {
        return op == '+' || op == '-';
    }

    bool isalnum_(char c) {
        return isalnum(c) || c == '_';
    }

    bool isdigit_or_dot(char c){
        return c=='.' || isdigit(c);
    }

    double calc (string const& s, const Memory<double>& m) {
        bool may_unary = false;
        vector<double> st;
        vector<char> op;
        for (size_t i=0; i<s.length(); ++i)
            if (!delim (s[i])) {
                if (s[i] == '(') {
                    op.push_back('(');
                    may_unary = true;
                } else if (s[i] == ')') {
                    while (op.back() != '(')
                        process_op(st, op.back()), op.pop_back();
                    op.pop_back();
                    may_unary = false;
                } else if (is_op(s[i])) {
                    char curop = s[i];
                    if (may_unary && isunary(curop)) curop = -curop;
                    while (!op.empty() && (
                            (curop >= 0 && priority(op.back()) >= priority(curop)) ||
                            (curop < 0 && priority(op.back()) > priority(curop)))
                            )
                        process_op(st, op.back()), op.pop_back();
                    op.push_back(curop);
                    may_unary = true;
                } else {
                    string operand;
                    while (i < s.length() && (isalnum_(s[i]) || isdigit_or_dot(s[i])))
                        operand += s[i++];
                    --i;
                    if (isdigit_or_dot(operand[0]))
                        st.push_back(strtod(operand.c_str(), nullptr));
                    else
                        st.push_back(m[operand]);
                    may_unary = false;
                }
            }
        while (!op.empty())
            process_op (st, op.back()),  op.pop_back();
        return st.back();
    }

    std::unordered_set<std::string> find_vars(std::string const& s) {
        std::unordered_set<std::string> vars;
        bool may_unary = false;
        vector<double> st;
        vector<char> op;
        for (size_t i=0; i<s.length(); ++i)
            if (!delim (s[i])) {
                if (s[i] == '(') {
                    op.push_back('(');
                    may_unary = true;
                } else if (s[i] == ')') {
                    while (op.back() != '(')
                        process_op(st, op.back()), op.pop_back();
                    op.pop_back();
                    may_unary = false;
                } else if (is_op(s[i])) {
                    char curop = s[i];
                    if (may_unary && isunary(curop)) curop = -curop;
                    while (!op.empty() && (
                            (curop >= 0 && priority(op.back()) >= priority(curop)) ||
                            (curop < 0 && priority(op.back()) > priority(curop)))
                            )
                        process_op(st, op.back()), op.pop_back();
                    op.push_back(curop);
                    may_unary = true;
                } else {
                    string operand;
                    while (i < s.length() && (isalnum_(s[i]) || isdigit_or_dot(s[i])))
                        operand += s[i++];
                    --i;
                    if (isdigit_or_dot(operand[0]))
                        st.push_back(strtod(operand.c_str(), nullptr));
                    else {
                        st.push_back(0);
                        vars.insert(operand);
                    }
                    may_unary = false;
                }
            }
        return vars;
    }

    RValueParser::RValueFunction RValueParser::get_rvalue_function(std::string const &rvalue) const {
        return [rvalue](const Memory<double> &m) -> double {
            if(rvalue == "") return 0;
            if(rvalue == "default") return 1;
            return bt::calc(rvalue, m);
        };
    }


    TreeParser::TreeParser(Tree* tree, std::string const &file) : tree(tree),
        file_(file), rvp(tree->get_memory()), nit(0) {}

    void TreeParser::load_field(const YAML::Node &node, std::string const &key, std::string &yo) const {
        if(node[key]) {
            try {
                yo =  node[key].as<std::string>();
            }
            catch(YAML::Exception &e){
                LOG_DEBUG("Error while reading " + key + " " + e.what());
            }
        }
    }

    std::unordered_set<std::string> TreeParser::load_node(std::string const &id, const YAML::Node &node) {

        if(!node.IsMap()) {
            throw YAML::Exception(YAML::Mark::null_mark(), "Not a map! : " + id);
        }

        std::string node_type(""), cls("");

        load_field(node, "type", node_type);
        load_field(node, "class", cls);
        if(node_type == "")
            throw YAML::Exception(YAML::Mark::null_mark(), "at node "+id);

        std::unordered_set<std::string> used_vars;

        if(node_type == "condition") {

            //conditionary functions
            std::set<std::string> sfr{"S","F","R"};
            Tree::dict<std::string> SFR{{"S",""},{"F",""},{"R",""}};


            for(auto const& f: node) {
                if(sfr.count(f.first.as<std::string>()))
                    SFR[f.first.as<std::string>()] = f.second.as<std::string>();
            }

            // used vars
            if(!node["used_vars"]) {
                for(auto const& f: SFR) {
                    if(f.second != "default") {
                        auto uv = find_vars(f.second);
                        used_vars.insert(uv.begin(), uv.end());
                    }
                }
            }
            else {
                if(node["used_vars"]) {
                    for(auto const& v: node["used_vars"]) {
                        used_vars.insert(v.as<std::string>());
                    }
                }
            }

            if(cls == "") {
                for(auto const& f:{"S","F","R"})
                    if(!SFR[f].empty())
                        cls += std::string(f) + ": " + SFR[f] + ";\n";
            }

            auto c_func = rvp.get_conditionary_function(SFR["S"], SFR["F"], SFR["R"]);
            store.add(id, new Condition(id, tree->get_memory(), c_func, used_vars, cls));
        }
        else if (node_type == "action") {
            if(!node["assign"].IsMap()) {
                throw YAML::Exception(YAML::Mark::null_mark(), "Not a map! : " + id);
            }
            auto const& assign = node["assign"];
            std::vector<Action::ActionaryFunction> assignments;
            bool add_classifier = (cls == "");
            try {
                for (auto const &p: assign) {
                    auto const &lvalue = p.first.as<std::string>();
                    std::string rvalue = p.second.as<std::string>();
                    assignments.push_back(rvp.get_actionary_function(lvalue, rvalue));
                    if(add_classifier)
                        cls += lvalue + " := " + rvalue + ";\n";
                    used_vars.insert(lvalue);
                    auto uv = find_vars(rvalue);
                    used_vars.insert(uv.begin(), uv.end());
                }
            }
            catch(YAML::Exception & e) {
                throw YAML::Exception(YAML::Mark::null_mark(), "failed assignments load at action " + id + " exc:" + e.what());
            }

            auto a_func = [assignments](Memory<double>& m) {
                for(auto const& f: assignments)
                    f(m);
            };

            store.add(id, new Action(id, tree->get_memory(), a_func, {}, cls));
        }
        else if(node_type == "latch") {
            std::string latch_var;
            load_field(node, "var", latch_var);
            if(latch_var == "")
                latch_var = id + "_var";
            std::string child;
            load_field(node, "child", child);
            if(child == "")
                throw YAML::Exception(YAML::Mark::null_mark(), "No child for Latch node "+id);
            graph[id] = {child};
            if(cls == "")
                cls = latch_var;
            store.add(id, new Latch(id, tree->get_memory(), latch_var, cls));
        }
        else if(std::set<std::string>{"sequence", "rssequence", "skipper", "selector", "parallel"}.count(node_type)) {
            std::vector<std::string> children;
            try {
                auto const& children_list = node["children"];
                for(auto const& c: children_list) {
                    children.push_back(c.as<std::string>());
                }
            }
            catch(YAML::Exception & e) {
                LOG_DEBUG("ar_bt_test : on parsing "+id + " " + e.what());
                throw YAML::Exception(YAML::Mark::null_mark(), e.what());
            }
            if(children.size() == 0) {
                throw YAML::Exception(YAML::Mark::null_mark(), "Empty children list at " + id);
            }

            graph[id] = children;


            if(node_type == "sequence") store.add(id, new Sequence(id, tree->get_memory(), cls));
            if(node_type == "selector") store.add(id, new Selector(id, tree->get_memory(), cls));
            if(node_type == "rssequence" || node_type == "skipper")
                store.add(id, new RunningSkippingSequence(id, tree->get_memory(), cls));
            if(node_type == "parallel") store.add(id, new Parallel(id, tree->get_memory(), cls));

            if(node["hide"] && node["hide"].as<int>()) {
                if(node_type == "sequence") store.Sequences[id]->hide_further = true;
                if(node_type == "selector") store.Selectors[id]->hide_further = true;
                if(node_type == "rssequence" || node_type == "skipper")
                    store.RunningSkippingSequences[id]->hide_further = true;
                if(node_type == "parallel") store.Parallels[id]->hide_further = true;
            }
        }
        else {
            throw YAML::Exception(YAML::Mark::null_mark(), "mistake in node type: " + node_type);
        }

        return used_vars;

    }

    std::unordered_set<std::string> TreeParser::load_nodes(const YAML::Node &ye) {
        auto const& nodes = ye;

        std::unordered_set<std::string> used_vars;
        for(auto const& p: nodes) {
            auto uv = load_node(p.first.as<std::string>(), p.second);
            used_vars.insert(uv.begin(), uv.end());
        }
        return used_vars;
    }

    void TreeParser::load_variables(const YAML::Node &ye) {
        auto const& variables = ye;
        Tree::dict<Memory<double>::Scope> scopes {
                {"input",Memory<double>::INNER},
                {"output",Memory<double>::OUTPUT},
                {"pure_output",Memory<double>::PURE_OUTPUT},
                {"output_no_send_zero",Memory<double>::OUTPUT_NO_SEND_ZERO}
        };

        for(auto const& p: variables) {
            auto const& sc_str = p.first.as<std::string>();
            if(!scopes.count(sc_str))
                throw YAML::Exception(YAML::Mark::null_mark(), "wrong scope name "+sc_str);
            auto scope = scopes[sc_str];

            try {
                for (auto const &v: p.second)
                    tree->get_memory().add_variable(v.as<std::string>(), scope);
            }
            catch(YAML::Exception & e) {
                throw YAML::Exception(YAML::Mark::null_mark(),"error while loading variables at scope " + sc_str);
            }
        }
    }


    void TreeParser::init_vars(const YAML::Node &ye) {
        auto const& vars = ye;
        for(auto const& p: vars) {
            try {
                std::string const& v_name = p.first.as<std::string>();
                double v_value = p.second.as<double>();
                tree->get_memory().set(v_name, v_value);
            }
            catch(YAML::Exception & e) {
                throw YAML::Exception(YAML::Mark::null_mark(), "error while initializing variables " + (std::string)e.what());
            }
        }
    }

    std::unordered_set<std::string> TreeParser::get_extra_vars(std::unordered_set<std::string> const &vars) {
        std::unordered_set<std::string> rest_vars;
        for(auto const& v_name: vars) {
            std::unordered_set<std::string> rest;
            if(!tree->get_memory().have(v_name))
                rest.insert(v_name);

            if(!rest.empty()) {
                std::string rest_vars;
                for(auto const& v: rest)
                    rest_vars += v;
            }
        }
        return rest_vars;

    }

#define ADD_TREE_NODE(type) if(store.type##s.count(child)) tree->add_node(parent, store.type##s[child]);

    void TreeParser::build_graph() {
        std::queue<std::string> nodes;
        nodes.push(tree->get_root_name());
        while(!nodes.empty()) {
            std::string parent = nodes.front();
            nodes.pop();
            for(auto const& child: graph[parent]) {
                FOR_EACH_TYPE(ADD_TREE_NODE);
                nodes.push(child);
            }
        }
    }


    void TreeParser::load_parameters(const YAML::Node &ye) {
        auto const& pars = ye;
        for(auto const& p: pars) {
            try {
                std::string const& v_name = p.first.as<std::string>();
                double v_value = p.second.as<int>();
                parameters[v_name] = v_value;
            }
            catch(YAML::Exception & e) {
                LOG_DEBUG(e.what());
                throw YAML::Exception(YAML::Mark::null_mark(), "error while initializing variables");
            }
        }
    }

    void TreeParser::load() {
        YAML::Node config;
        try {
            config = YAML::LoadFile(file_);
        }
        catch(YAML::Exception &e) {
            throw YAML::Exception(YAML::Mark::null_mark(), "smth wrong with tree description file " + (std::string)e.what());
        }

        auto const& name = tree->get_root_name();
        auto const& info = config["common"];
        std::string root_child;
        load_field(info, "root_child", root_child);
        if(root_child == "") {
            throw YAML::Exception(YAML::Mark::null_mark(), "no child of root wrote!");
        }

        graph[name] = {root_child};

        //TODO:

        if(info["parameters"])
            load_parameters(info["parameters"]);

//            YamlMapping const& my_info = config[name].mapping();


        load_variables(config["variables"]);

        // TODO: match all variables to nodes
        for(auto const& scope: config["nodes"]) {
            auto rest_vars = get_extra_vars(load_nodes(scope.second));
            if(!rest_vars.empty()) {
                std::string rest;
                for(auto const& v: rest_vars)
                    rest += v + ' ';
                throw YAML::Exception(YAML::Mark::null_mark(), "variables were not created: " + rest);
            }
        }

        build_graph();


        if(config["set"])
            init_vars(config["set"]);

    }

    std::string TreeParser::apply_sample(const YAML::Node &ye) {
        auto const& vars = ye;
        Tree::dict<double> changes;
        for(auto const& p: vars) {
            try {
                std::string const& v_name = p.first.as<std::string>();
                double v_value = p.second.as<double>();
                changes[v_name] = v_value;
            }
            catch(YAML::Exception &e) {
                throw YAML::Exception(YAML::Mark::null_mark(), "ar_bt_test: sample rvalue is not double");
            }
        }
        auto out = tree->callback(changes);
        std::string output;
        for(auto const& p: out) {
          if (p.first != "#sync_ended")
            output += p.first + ": " + std::to_string(p.second) + "\n";
        }
        return output;
    }

    std::string TreeParser::apply_samples(std::string const &file) {
        YAML::Node samples;
        try {
            samples = YAML::LoadFile(file);
        }
        catch(YAML::Exception &e) {
            throw YAML::Exception(YAML::Mark::null_mark(),"smth wrong with samples file " + (std::string)e.what());
        }
        std::string result;
        if(!samples["samples"]) {
            LOG_DEBUG("no samples provdided!");
        }
        else {
            for (auto const &p: samples["samples"]) {
                result += apply_sample(p)  + "-------------------------\n";
            }
        }
        return result;


    }

    std::string TreeParser::get_graph_viz_description() const {
        return tree->dot_tree_description();
    }

    void TreeParser::load_changes_from_str(std::string const &str) {

    }
};
