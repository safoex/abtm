//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_DEFS_H
#define ABTM_DEFS_H

#include <unordered_map>
#include <any>
#include <functional>

#define SCREAM(x) for(int i = 0; i < 20; i++) std::cout << x; std::cout << std::endl;

namespace bt {
    template<typename T> using dictOf = std::unordered_map<std::string, T>;
    typedef dictOf<std::any> sample;
    typedef std::function<sample(sample const&)> ExternalFunction;
    typedef std::function<void(sample const&)> InputFunction;
    enum VarScope {
        INNER,
        OUTPUT,
        PURE_OUTPUT,
        OUTPUT_NO_SEND_ZERO
    };
};

#endif //ABTM_DEFS_H
