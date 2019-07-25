//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_DEFS_H
#define ABTM_DEFS_H

#include <unordered_map>
#include <any>
#include <functional>

namespace bt {
    template<typename T> using dictOf = std::unordered_map<std::string, T>;
    typedef dictOf<std::any> sample;
    typedef std::function<sample(sample const&)> ExternalFunction;
};

#endif //ABTM_DEFS_H
