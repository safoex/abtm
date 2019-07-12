//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_DEFS_H
#define ABTM_DEFS_H

#include <unordered_map>
#include <any>

namespace bt {
    template<typename T> using dictOf = std::unordered_map<std::string, T>;
};

#endif //ABTM_DEFS_H
