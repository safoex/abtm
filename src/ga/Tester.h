//
// Created by safoex on 21.08.19.
//

#ifndef ABTM_TESTER_H
#define ABTM_TESTER_H

#include "defs.h"

using namespace bt;
typedef std::pair<sample, std::vector<std::pair<sample, sample>> > test;

namespace bt {
    class Test {
    public:
        test t;
        enum SAMPLE_COMPARISON {
            DIFFERENT,
            SIMILAR_VAR,
            SIMILAR_VAL,
            SIMILAR,
            SIMILAR_EMPTY
        };
        const int scores[5]{0, 10, 5, 40, 3};
        const double combo = 1.5;

        Test() {};

        Test(test const &t) : t(t) {};

        void print_test() {
            std::cout << "start: " << std::endl;
            for (auto const &kv: t.first) {
                std::cout << '\t' << kv.first << ": " << std::any_cast<std::string>(kv.second) << std::endl;
            }
            std::cout << std::endl;
            for (auto const &p : t.second) {
                auto const &in = p.first, out = p.second;
                std::cout << "1: " << std::endl;
                std::cout << "\tin: " << std::endl;
                for (auto const &kv: in) {
                    std::cout << kv.first << std::endl;
                    int status;
                    std::cout << "\t\t" << kv.first << ": " << std::any_cast<double>(kv.second) << std::endl;
                }
                std::cout << "\tout: " << std::endl;
                for (auto const &kv: out) {
                    std::cout << kv.first << std::endl;
                    std::cout << "\t\t" << kv.first << ": " << std::any_cast<std::string>(kv.second) << std::endl;
                }
                std::cout << std::endl;
            }
        }

        bool has_output() {
            bool has = !t.first.empty();
            for (auto const &ps: t.second) {
                has |= !ps.second.empty();
            }
            return has;
        }

        test apply_test(Tree *tree) const {
            test result({}, std::vector<std::pair<sample, sample>>(t.second.size(), {{},
                                                                                     {}}));
            result.first = tree->start();
            for (int i = 0; i < t.second.size(); i++) {
                result.second[i].first = t.second[i].first;
                result.second[i].second = tree->callback(t.second[i].first);
            }
            return result;
        }

        SAMPLE_COMPARISON compare(sample const &a, sample const &b) const {
            // only 1 output
            if (a.empty() && b.empty()) {
                return SAMPLE_COMPARISON::SIMILAR_EMPTY;
            } else if (!a.empty() && !b.empty()) {
                auto const &var_a = a.begin()->first, var_b = b.begin()->first;
                auto const &val_a = std::any_cast<std::string>(a.begin()->second),
                        val_b = std::any_cast<std::string>(b.begin()->first);
                return SAMPLE_COMPARISON(int(var_a == var_b) + (val_a == val_b) * 2);
            } else {
                return SAMPLE_COMPARISON::DIFFERENT;
            }
        }

        double test_to_test_fitness(Test const &a) const {
            double c = 1;
            double score = 0;
            auto scomp = compare(a.t.first, t.first);
            if (scomp == SAMPLE_COMPARISON::SIMILAR) {
                c *= combo;
            } else c = 1;
            score += scores[scomp];
            for (int i = 0; i < a.t.second.size(); i++) {
                auto scomp = compare(a.t.second[i].second, t.second[i].second);
                if (scomp == SAMPLE_COMPARISON::SIMILAR) {
                    c *= combo;
                } else c = 1;
                score += scores[scomp];
            }
            return score;
        }

        double tree_to_test_fitness(Tree *tree) const {
            auto new_t = apply_test(tree);
            return test_to_test_fitness(new_t);
        }

    };
}
#endif //ABTM_TESTER_H
