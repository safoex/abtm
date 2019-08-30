//
// Created by safoex on 21.08.19.
//

#ifndef ABTM_SIMPLIFIEDBUILDER_H
#define ABTM_SIMPLIFIEDBUILDER_H

#include "Tree.h"
#include "memory/MemoryJS.h"

namespace bt {
    class SimplifiedBuilder {
    public:
        int height;
        int inputs, outputs, extra;
        int variables_in_conditions;
        int limit;
        int controls;
        int leafs;
        int total_vars;
        std::vector<Node*> current;
        const std::vector<std::string> operators{"==", "+", "-", "&&", "||", ">", "<"};
        SimplifiedBuilder(int height, int inputs, int outputs, int extra, int limit)
            : height(height), inputs(inputs), outputs(outputs), extra(extra), limit(limit) {
            variables_in_conditions = 2;
            controls = (1<<height)-1;
            leafs = controls + 1;
            total_vars = inputs + outputs + extra;
        }

        Sequential* control_node(MemoryBase* m, int type, std::string const& name) {
            Sequential* node = nullptr;
            switch(type) {
                case Node::Sequence: node =  new Sequence(name, *m);
                case Node::Selector: node =  new Selector(name, *m);
                case Node::RunningSkippingSequence: node =  new RunningSkippingSequence(name, *m);
                case Node::Parallel: node =  new Parallel(name, *m);
                default: node =  new Sequence(name, *m);
            }
            current.push_back(node);
            return node;
        }

        void req_add_children(Tree* t, MemoryBase* m, std::vector<int> const& x, int p, std::string const& parent, int order) {
            auto name = std::to_string(p);
            t->add_node(parent, control_node(m, x[p], name));
            if(order+1 < height) {
                req_add_children(t,m,x,2*(p+1)-1,name,order+1);
                req_add_children(t,m,x,2*(p+1),name,order+1);
            }
        }

        void build_controls(Tree* t, MemoryBase* m, std::vector<int> const& x) {
            req_add_children(t,m,x,0,"r",0);
        }

        std::string get_var(int x) {
            std::string z(" ");
            z[0] = x + 'A';
            std::cout << z << std::endl;
            return z;
        }

        Condition* build_condition(MemoryBase* m, std::vector<int> const& x, int from) {
            int TrueState = x[from] / 3, FalseState = x[from] % 3;
            std::unordered_set<std::string> used_vars;
            std::string expr = get_var(x[from+1]);
            used_vars.insert(expr);
            for(int i = 0; i < variables_in_conditions-1; i++) {
                int var = x[from+2 + 2*i + 1];
                int op  = x[from+2 + 2*i + 0];
                expr += operators[op] + get_var(var);
                used_vars.insert(get_var(var));
            }

            Condition::ConditionaryFunction f = [expr, TrueState, FalseState](MemoryBase& mem) ->Node::State {
                if(mem.eval_bool(expr)) return Node::State(TrueState);
                else return Node::State(FalseState);
            };
            std::string cls = STATE(TrueState) + ": " + expr + '\n' + STATE(FalseState) + ": default";
            Condition* node = new Condition(std::to_string(from),*m, f, used_vars, cls);
            current.push_back(node);
            return node;
        }

        Action* build_action(MemoryBase* m, std::vector<int> const& x, int from) {
            int var_assign = x[from];
            int v1 = x[from+1],v2 = x[from+3], op = x[from+2];
            std::string expr = get_var(v1) + operators[op] + get_var(v2);
            std::string lvalue = get_var(var_assign);
            Action::ActionaryFunction f = [lvalue, expr](MemoryBase& mem) {
                mem.set_expr(lvalue, expr);
            };
            std::string cls = lvalue + " = " + expr;
            Action* node;
            node =  new Action(std::to_string(from),*m,f,{}, cls);
            current.push_back(node);
            return node;
        }

        void add_vars(MemoryBase* m, std::vector<int> const& x, int init_from) {
            for(int i = 0; i < inputs; i++) {
                m->add(get_var(i), VarScope::INNER, double(0));
            }
            for(int i = inputs; i < inputs + outputs; i++) {
                m->add(get_var(i), VarScope::OUTPUT, double(0));
            }
            for(int i = inputs + outputs, k = init_from; i <inputs + outputs + extra; i++, k++) {
                m->add(get_var(i), VarScope::INNER, double(x[k]));
            }
        }

        void build_leafs(Tree* t, MemoryBase* m, std::vector<int> const& x, int leafs_from) {
            int leafs_total = 1 << height;
            for(int i = 0; i < leafs_total; i++) {
                int k = i*5 + leafs_from;
                if(x[k]) {
                    t->add_node(std::to_string(leafs_total/2 - 1 + i/2), build_action(m,x, k + 1));
                }
                else t->add_node(std::to_string(leafs_total/2 - 1 + i/2), build_condition(m,x, k+1));
            }
        }

        void clear_current() {
            for(auto n: current) {
                delete n;
            }
            current.clear();
        }

        std::pair<MemoryJS*, Tree*> get_sample(std::vector<int> const& x) {
            clear_current();
            auto memory = new MemoryJS;
            auto tree = new Tree(*memory, "r");
            build_controls(tree, memory, x);
            int leafs_from = (1<<height)-1;
            int leafs_total = 1<<height;
            int init_from = 5 * leafs_total + leafs_from;
            add_vars(memory, x, init_from);
            build_leafs(tree, memory, x, leafs_from);
            return {memory, tree};
        }

        int get_x_size() {
            return  (1<<height) - 1 +
                    (1<<height) * 5 +
                    extra;
        }
    };
}


#endif //ABTM_SIMPLIFIEDBUILDER_H
