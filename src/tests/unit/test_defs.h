//
// Created by safoex on 05.08.19.
//

#ifndef ABTM_TEST_DEFS_H
#define ABTM_TEST_DEFS_H

#define REVEAL(X) {\
std::string test = X;\
if (success)\
std::cout << test + " : OK\n";\
else {\
std::cout << test + " : ERROR\n";\
return 1;\
}\
}

#include <unordered_set>

void print_set(std::unordered_set<std::string> s) {
    for(auto const& st: s)
        std::cout << st << '\t';
    std::cout << std::endl;
}

#endif //ABTM_TEST_DEFS_H
