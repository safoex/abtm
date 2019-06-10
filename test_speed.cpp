//
// Created by safoex on 25.05.19.
//

#include <yaml-cpp/yaml.h>
#include <Tree.h>
#include <TreeParser.h>
#include <omp.h>


#include <iostream>
#include <chrono>
#include <cstdlib>

template<typename TimeT = std::chrono::microseconds>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
    {
        auto start = std::chrono::steady_clock::now();
        std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast< TimeT>
                (std::chrono::steady_clock::now() - start);
        return duration.count();
    }
};

#include <random>


namespace bt{
    class BTTesting {
        friend class Tree;
        friend class TreeParser;

    public:
        Tree t;
        TreeParser tp;
        std::random_device rnd;
        std::vector<std::string> oper;

        BTTesting() : t("test"), tp(&t, ""), oper{"&","|"} {}
        std::string get_random_var(unsigned long which) {
            auto w = std::to_string(which);
            for(auto &c: w) c = 'a' - '0' + c;
            return w;
        }

        std::string create_random_expr(unsigned vars, unsigned vars_amount) {
            std::string F;
            for(int k = 0; k < vars-1; k++)
                F += std::string() + get_random_var(rnd()%vars_amount) + " " + oper[rnd()%oper.size()] + " ";
            F += get_random_var(rnd()%vars_amount);
            return F;
        }

        void create_random_tree(int depth, int children_per_node, int condition_complexity,
                int parallel_layers, int vars_amount) {
            std::string ALPHABET, alphabet;
            std::vector<std::string> controlnodes{"parallel", "sequence", "selector","rssequence"};
            for(char c = 'A'; c <= 'Z'; c++)
                ALPHABET.push_back(c), alphabet.push_back(c-'A'+'a');
            std::vector<std::string> nexts = {""}, nexts2;
            for(int i = 0; i < depth; i++) {
                for(auto const& n: nexts) {
                    for(int j = 0; j < children_per_node; j++) {
                        std::string name = n + ALPHABET[j];
                        std::string type;
                        if(i < parallel_layers) {
                            type = "parallel";
                        }
                        else {
                            type = controlnodes[rnd() % 4];
                        }
                        YAML::Node node, children;
                        node["type"] =  type;
                        for(int k = 0; k < children_per_node; k++)
                            children.push_back(name+ALPHABET[k]);
                        node["children"] = children;
                        tp.load_node(name,node);
                        nexts2.push_back(name);
                    }
                }
                nexts = nexts2;
                nexts2.clear();
            }

            for(auto const& n: nexts) {
                for (int j = 0; j < children_per_node; j++) {
                    std::string name = n + ALPHABET[j];
                    std::string type = "condition";
                    std::string S,F,R="default";


                    int vars_in_cond = condition_complexity;
                    S = create_random_expr(rnd()%vars_in_cond+1,vars_amount);
                    F = create_random_expr(rnd()%vars_in_cond+1,vars_amount);

                    YAML::Node node;
                    node["type"] = type;
                    node["S"] = S; node["F"] = F; node["R"] = R;
                    tp.load_node(name, node);
                }
            }
            std::vector<std::string> fc;
            std::string no;
            for(int i = 0; i < children_per_node; i++) {
                fc.push_back(no + ALPHABET[i]);
            }
            tp.graph[t.get_root_name()] = fc;

            tp.build_graph();
        }
    };
};

using namespace bt;


int main() {

//    Tree t("test");
//    TreeParser tp(&t, "../config/test_rep.yaml");
//    tp.load();
//
//    std::ofstream test_gv("test_gv_tree.txt");
//    test_gv << t.dot_tree_description();
//
//
//    auto rep_async = [&t](int times) {
//
//    };




    auto rep_async = [](int repeat, int samples, int depth, int children_per_node, int vars_in_cond,
            int parallel_layers, int vars) -> long  {
        //std::ofstream("rand.txt") << btt.t.dot_tree_description();
        //system("dot -Tpdf rand.txt > rand.pdf");
        std::random_device rnd;
        long time = 0;
        for(int _i = 0; _i < repeat; _i++) {
            BTTesting btt;
            btt.create_random_tree(depth, children_per_node, vars_in_cond, parallel_layers, vars);
            std::vector<std::string> keys;
            for (auto &k: btt.t.get_memory().var)
                keys.push_back(k.first);
            for (auto const &k: keys)
                btt.t.get_memory().var[k] = rnd() % 2;
            btt.t.start();


            std::vector<Tree::dict<double>> ss;
            ss.resize(samples);
            for(int j = 0; j < samples; j++) ss[j] = {{keys[rnd() % keys.size()], rnd() % 2}};

            time +=
            measure<>::execution([&]() {
                for (int j = 0; j < samples; j++) {
                    btt.t.callback(ss[j]);
                }
            });
        }
        return time;
    };

    auto rep_sync = [](int repeat, int samples, int depth, int children_per_node, int vars_in_cond,
            int parallel_layers, int vars) -> long {
        //std::ofstream("rand.txt") << btt.t.dot_tree_description();
        //system("dot -Tpdf rand.txt > rand.pdf");
        std::random_device rnd;
        long time = 0;
        for(int _i = 0; _i < repeat; _i++) {
            BTTesting btt;
            btt.create_random_tree(depth, children_per_node, vars_in_cond, parallel_layers, vars);
            std::ofstream("rand.txt") << btt.t.dot_tree_description();
            system("dot -Tpdf rand.txt > rand.pdf");

            std::vector<std::string> keys;
            for (auto &k: btt.t.get_memory().var)
                keys.push_back(k.first);
            for (auto const &k: keys)
                btt.t.get_memory().var[k] = rnd() % 2;
            btt.t.start();

            std::vector<Tree::dict<double>> ss;
            ss.resize(samples);
            for(int j = 0; j < samples; j++) ss[j] = {{keys[rnd() % keys.size()], rnd() % 2}};

            time +=
            measure<>::execution([&]() {
                for (int j = 0; j < samples; j++) {
                    btt.t.get_memory().set(ss[j]);
                    btt.t.start();
                }
            });
        }
        return time;
    };

    auto rep_both = [](int repeat, int samples, int depth, int children_per_node, int vars_in_cond,
                       int parallel_layers, long vars) -> std::tuple<long,long,long> {
        std::random_device rnd;
        long time_sync = 0, time_async = 0, time_async_nocb = 0;
        for(int _i = 0; _i < repeat; _i++) {
            BTTesting btt;
            btt.create_random_tree(depth, children_per_node, vars_in_cond, parallel_layers, vars);
            std::vector<std::string> keys;
            for (auto &k: btt.t.get_memory().var)
                keys.push_back(k.first);
            for (auto const &k: keys)
                btt.t.get_memory().var[k] = rnd() % 2;
            auto init_var = btt.t.get_memory().var;

            std::vector<Tree::dict<double>> ss;
            ss.resize(samples);
            for(int j = 0; j < samples; j++) {
                auto k = keys[rnd() % keys.size()];
                ss[j] = {{k, 1 - init_var[k]}};
            }


            btt.t.start();
            time_sync +=
                measure<>::execution([&]() {
                    for (int j = 0; j < samples; j++) {
                        btt.t.get_memory().set(ss[j]);
                        btt.t.start();
                    }
                });

            btt.t.get_memory().var = init_var;
            btt.t.start();
            time_async +=
                measure<>::execution([&]() {
                    for (int j = 0; j < samples; j++) {
                        btt.t.callback(ss[j]);
                    }
                });

            btt.t.get_memory().var = init_var;
            btt.t.start();
            std::string k = btt.t.get_memory().var.begin()->first;
            double d = btt.t.get_memory().var[k];
            time_async_nocb +=
                    measure<>::execution([&]() {
                        for (int j = 0; j < samples; j++) {
                            btt.t.callback({{k, d}});
                        }
                    });

        }
        return {time_async, time_sync, time_async_nocb};
    };

    long trees = 300,
        repeats = 3000,
        depth = 4,
        children = 4,
        vars_in_condition = 4,
        parallel_layers = 0,
        diff_vars = 100000000;
//    std::cout << measure<>::execution([&]() {
//        std::cout << rep_async(trees, repeats, depth, children, vars_in_condition, parallel_layers, diff_vars)
//                  << std::endl;
//        std::cout << rep_sync(trees, repeats, depth, children, vars_in_condition, parallel_layers, diff_vars)
//                  << std::endl;
//    }) << std:: endl;



    rep_sync(1,1,2,2,4,0,10);

//    std::ofstream T("tests16.txt");
//
//
//
//    T << "trees" << '\t' << "repeats" << '\t' << "depth" << '\t' << "children" << '\t'
//        << "condition" << '\t' << "parallel" << '\t'  << "diff" << '\t'
//            << "async" << '\t' << "classic" << '\t' << "omit" << '\t'  << std::endl;
//    std::mutex lock;
//    std::cout << measure<>::execution([&]() {
//        for(depth = 2; depth <= 4; depth++)
//            for(children = 3; children <= 10/depth+3; children+=1)
//               for(vars_in_condition = 2; vars_in_condition <= 8; vars_in_condition += 2)
//                for(diff_vars = 100; diff_vars <= 10000000; diff_vars *= 4)
//                {
//
//                    auto both = rep_both(trees, repeats, depth, children, vars_in_condition, parallel_layers,
//                                         diff_vars);
//                    {
//                        std::lock_guard<std::mutex> lg(lock);
//                        for (auto v: {trees, repeats, depth, children, vars_in_condition, parallel_layers, diff_vars}) {
//                            T << v << '\t';
//                            std::cout << v << '\t';
//                        }
//                        T << std::get<0>(both) << '\t' << std::get<1>(both) << '\t' << std::get<2>(both) << std::endl;
//                        std::cout << std::get<0>(both) << '\t' << std::get<1>(both) << '\t' << std::get<2>(both) << '\t'
//                                  << std::endl;
//                    }
//                }
//    }) << std:: endl;

    return 0;
}