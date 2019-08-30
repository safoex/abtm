//
// Created by safoex on 21.08.19.
//

#include "SimplifiedBuilder.h"
#include <random>
#include <cxxabi.h>
#include "openga.hpp"
#include "Tester.h"
#include <set>

std::random_device rnd;
using namespace bt;

void create_random_action(SimplifiedBuilder& b, std::vector<int>& x, int k) {
    x[k+1] = rnd() % b.total_vars; // var A =
    x[k+2] = rnd() % b.total_vars; // var B
    x[k+3] = rnd() % b.operators.size(); // op +
    x[k+4] = rnd() % b.total_vars; // var C
}

void create_random_condition(SimplifiedBuilder& b, std::vector<int> &x, int k) {
    x[k+1] = rnd() % 9; // nine types of SF bla bla
    x[k+2] = rnd() % b.total_vars; // D
    x[k+3] = rnd() % b.operators.size(); // &&
    x[k+4] = rnd() % b.total_vars; // E
}

std::vector<int> create_random_x(SimplifiedBuilder& b) {
    std::vector<int> x(b.get_x_size(),0);
    int leafs_from = (1<<b.height)-1;
    int leafs_total = 1<<b.height;
    int init_from = 5 * leafs_total + leafs_from;

    // random control nodes

    for(int i = 0; i < leafs_from; i++)
        x[i] = rnd()%4; // type of control nodes;

    int total_vars = b.inputs + b.outputs + b.extra;
    int operators = b.operators.size();

    // random leaf nodes

    for(int i = 0; i < leafs_total; i++) {
        int k = i*5 + leafs_from;
        x[k] = rnd() % 2; // type of leaf node

        if(x[k]) { // if action
            create_random_action(b, x, k);
        }
        else { // if condition
            create_random_condition(b, x, k);
        }
    }

    // random init extra vars

    for(int i = init_from; i < x.size(); i++) {
        x[i] = rnd() % b.limit;
    }

    return x;
}

test create_random_test(SimplifiedBuilder& b, Tree* tree, int samples) {
    sample first = tree->start();
    std::vector<std::pair<sample, sample>> result(samples, {{},{}});
    for(int i = 0; i < samples; i++) {
        sample in;
//        int vars_in_input = rnd() % b.inputs + 1;
        int vars_in_input = 1;
        //
        in[b.get_var(rnd() % b.inputs)] = double(rnd() % b.limit);
        auto out = tree->callback(in);
        result[i] = {in, out};
    }
    return {first, result};
}

typedef std::unordered_map<std::string, int> sample_int;

//sample_int convert(sample const& s) {
//    sample_int S;
//    for(auto const& kv: s) {
//        S[kv.first] =
//    }
//}

struct MySolution
{
    std::vector<int> x;

    std::string to_string() const
    {
        std::string res;
        for(int i = 0; i < x.size(); i++) {
            res += std::to_string(x[i]) + ' ';
        }
        return res;
    }
};

struct MyMiddleCost
{
    // This is where the results of simulation
    // is stored but not yet finalized.
    double cost;
};


typedef std::vector<int> TesterSolution;
typedef double TesterCost;

typedef EA::Genetic<TesterSolution,TesterCost> GA_Type;
typedef EA::GenerationType<TesterSolution,TesterCost> Generation_Type;
class GATester {
public:
    SimplifiedBuilder b;
    std::vector<Test> tests;
    int total_tests, samples_in_test;
    std::ofstream output;
    GATester( int height, int inputs, int outputs, int extra, int limit, Tree* t, int total_tests, int samples_in_test)
        : b(height, inputs, outputs, extra, limit), total_tests(total_tests), samples_in_test(samples_in_test) {
        tests.resize(total_tests);
        for(int i = 0; i < total_tests; i++ ) {
            tests[i].t = create_random_test(b, t, samples_in_test);
        }
    }

    std::function<void(TesterSolution&, const std::function<double()>&)> get_init_genes_function() {
        return [this](TesterSolution& p,const std::function<double(void)> &rnd01) {
            p = create_random_x(this->b);
        };
    }

    std::function<bool(const TesterSolution&, TesterCost&)> get_eval_solution_function() {
        return [this]( const TesterSolution& p,
                TesterCost &c) -> bool {
            c = 0;
            for(auto const& test: tests) {
                auto [mem, tree] = b.get_sample(p);
                c += test.tree_to_test_fitness(tree);
                delete mem;
                delete tree;
            }
            return true;
        };
    }

    std::function<TesterSolution(const TesterSolution&, const std::function<double(void)> &, double)> get_mutate() {
        std::function<int()> get_mutation_type = []() -> int {
            constexpr int N = 4;
            constexpr int weights[N] = {10, 10, 10, 10};
            int mutation = rnd() % 40;
            for(int i = 0; i < N; i++) {
                if(mutation < weights[i]) {
                    return i;
                }
                else mutation -= weights[i];
            }
            return N;
        };
        return [this, get_mutation_type](const TesterSolution& X_base, const std::function<double(void)> &rnd01, double shrink_scale) -> TesterSolution {
            int mutation_type = get_mutation_type();
            auto X_new = X_base;
            if(mutation_type == 1) { // change some control node
                int node = rnd() % b.controls;
                X_new[node] = rnd() % 4;
            }
            else if (mutation_type == 2) { // change some action to condition
                int node = rnd() % b.leafs;
                int k = node * 5 + b.controls;
                X_new[k] = 1 - X_new[k];
                if(X_new[k]) {
                    create_random_action(b, X_new, k);
                }
                else create_random_condition(b, X_new, k);

            } else if (mutation_type == 3) { // change vars / vals in action / condition
                int node = rnd() % b.leafs;
                int k = node * 5 + b.controls;

                int which = rnd() % 4;
                int dir = 2 * (rnd() % 2) - 1;

                X_new[k + which + 1] += dir;
                int div;
                if(X_new[k]) {
                    if(which == 2) {
                        div =  b.operators.size();
                    }
                    else div = b.total_vars;
                    X_new[k + which + 1] = (X_new[k + which + 1] + div) % div;
                }
                else {
                    int div;
                    switch(which) {
                        case 0: div = 9; break;
                        case 1: div = b.total_vars; break;
                        case 2: div = b.operators.size(); break;
                        case 3: div = b.total_vars; break;
                        default: div = 9;
                    }
                    X_new[k + which + 1] = (X_new[k + which + 1] + div) % div;
                }
            } else {
                if (b.extra != 0) { // change init vars
                    int which = rnd() % b.extra;
                    int dir = 2 * ( rnd() % 2 ) - 1;
                    int div = b.limit;
                    X_new[b.controls + b.leafs + which] += dir;
                    X_new[b.controls + b.leafs + which + 1] = (X_new[b.controls + b.leafs + which + 1] + div) % div;
                }
            }
            return X_new;
        };
    }



    std::function<TesterSolution(const TesterSolution&, const TesterSolution&, const std::function<double()>)> get_crossover() {
        return [this](const TesterSolution& X1,
                      const TesterSolution& X2,
                      const std::function<double(void)> &rnd01) -> TesterSolution {

            TesterSolution X_new(X2);
            int from = rnd() % (b.controls - 1) + 1;
            std::vector<int> all, current, next;
            current.push_back(from);
            while(true) {
                for (auto k : current) {
                    next.push_back(2 * (k + 1));
                    next.push_back(2 * (k + 1) - 1);
                    all.push_back(k);
                }
                if(next[next.size() - 1] >= b.controls) {
                    break;
                }
                current = next;
                next.clear();
            }
            auto const& leafs = next;
            for(int n: all) {
                X_new[n] = X1[n];
            }
            for(int f: leafs) {
                for(int k = 0; k < 5; k++) {
                    X_new[5*f + b.controls + k] = X1[5*f + b.controls + k];
                }
            }
            return X_new;
        };
    }

    std::function<double(const GA_Type::thisChromosomeType)> calculate_objective() {
        return [this](const GA_Type::thisChromosomeType X) -> double {
            return X.middle_costs;
        };
    }

    std::function<void(int, const EA::GenerationType<TesterSolution,TesterCost > &, const TesterSolution&)> report_generation()
    {
        return [this](int generation_number,
                      const EA::GenerationType<TesterSolution,TesterCost > &last_generation,
                      const TesterSolution& best_genes) {
            std::cout
                    << "Generation [" << generation_number << "], "
                    << "Best=" << last_generation.best_total_cost << ", "
                    << "Average=" << last_generation.average_cost << ", "
                    //<<"Best genes=("<<best_genes.to_string()<<")"<<", "
                    << "Exe_time=" << last_generation.exe_time
                    << std::endl;

            output
                    << generation_number << "\t";
            for (auto x: best_genes) output << x << " ";
            //<<best_genes.y<<"\t"
            output << last_generation.average_cost << "\t"
                   << last_generation.best_total_cost << "\n";
        };
    }
};



int main() {
    SimplifiedBuilder b(2,2,1,2,10);

    bool still_empty = true;
    Tree* tree = nullptr;
    std::vector<int> res_x;
    while(still_empty) {
        auto x = create_random_x(b);
        auto s = b.get_sample(x);
        s = b.get_sample(create_random_x(b));
        for(int i = 0; i < 10; i++) {
            auto t = create_random_test(b, s.second, 10);
            still_empty = Test(t).has_output();
            if(!still_empty) {
                tree = s.second;
                res_x = x;
            }
        }
    }

    std::ofstream test_gv("test_gv_tree.txt");
//    test_gv << builder.get_dot_description(Builder::DOT) << std::endl;
    test_gv << tree->dot_tree_description(false) << std::endl;
    system("dot -Tpdf test_gv_tree.txt > tree.pdf");


    EA::Chronometer timer;
    timer.tic();

    GATester GAT(2,2,1,2,2, tree, 10, 10);


    GA_Type ga_obj;
    ga_obj.problem_mode= EA::GA_MODE::SOGA;
    ga_obj.multi_threading=false;
    ga_obj.idle_delay_us=1; // switch between threads quickly
    ga_obj.verbose=false;
    ga_obj.population=20;
    ga_obj.generation_max=1000;
    ga_obj.calculate_SO_total_fitness=GAT.calculate_objective();
    ga_obj.init_genes=GAT.get_init_genes_function();
    ga_obj.eval_solution= GAT.get_eval_solution_function();
    ga_obj.mutate= GAT.get_mutate();
    ga_obj.crossover= GAT.get_crossover();
    ga_obj.SO_report_generation= GAT.report_generation();
    ga_obj.best_stall_max=10;
    ga_obj.elite_count=10;
    ga_obj.crossover_fraction=0.7;
    ga_obj.mutation_rate=0.4;
    ga_obj.solve();
}