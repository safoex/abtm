//
// Created by safoex on 02.08.19.
//

#ifndef ABTM_MEMORYBASE_H
#define ABTM_MEMORYBASE_H

#include <defs.h>
#include <string>
#include <any>

namespace bt {
    class MemoryBase {
    public:
        MemoryBase() = default;
        virtual void add(std::string const& key, VarScope scope) = 0;
        virtual void add(std::string const& key, VarScope scope, std::any const& init) = 0;
        virtual void set(std::string const& key, std::any const& value) = 0;
        virtual std::any get(std::string const& key)  = 0;
        virtual sample get_changes(VarScope scope) = 0;
        virtual void clear_changes(VarScope scope) = 0;
        virtual void set_expr(std::string const& key, std::string const& expr) = 0;
        virtual void eval(std::string const& expr) = 0;
        virtual ~MemoryBase() = default;
    };
}

#endif //ABTM_MEMORYBASE_H
