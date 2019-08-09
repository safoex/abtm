//
// Created by safoex on 23.07.19.
//

#ifndef ABTM_EXECUTORBASE_H
#define ABTM_EXECUTORBASE_H

#include "memory/MemoryBase.h"
#include "defs.h"
#include <any>

namespace bt {
    class ExecutorBase {
    public:
        ExecutorBase() = default;
        virtual sample callback(sample const& input) = 0;
        virtual sample init() = 0;
        virtual sample& update_sample(sample& s) = 0;
        virtual sample update_sample(sample const& s) const = 0;
        virtual MemoryBase& get_memory() = 0;
        virtual ~ExecutorBase() = default;
    };
}

#endif //ABTM_EXECUTORBASE_H
