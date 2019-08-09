//
// Created by safoex on 02.08.19.
//

#ifndef ABTM_MEMORYBASE_H
#define ABTM_MEMORYBASE_H

#include <defs.h>
#include <string>
#include <any>
#include <unordered_set>

namespace bt {
    class MemoryBase {
    public:
        MemoryBase() = default;
        virtual void add(std::string const& key, VarScope scope) = 0;
        virtual void add(std::string const& key, VarScope scope, std::any const& init) = 0;
        virtual void set(std::string const& key, std::any const& value) = 0;
        virtual std::any get(std::string const& key)  = 0;
        virtual std::optional<double> get_double(std::string const& key) = 0;
        virtual std::optional<bool> get_bool(std::string const& key) = 0;
        virtual std::optional<std::string> get_string(std::string const& key) = 0;
        virtual sample get_changes(VarScope scope) = 0;
        virtual void clear_changes(VarScope scope) = 0;
        virtual void set_expr(std::string const& key, std::string const& expr) = 0;
        virtual void eval_action(std::string const& expr) = 0;
        virtual void eval(std::string const& expr) = 0;
        virtual bool has_var(std::string const& key) = 0;
        virtual bool eval_bool(std::string const& expr) = 0;
        virtual bool test_expr(std::string const& expr) = 0;
        virtual std::unordered_set<std::string> const& get_known_vars() = 0;
        virtual ~MemoryBase() = default;
    };
}

#endif //ABTM_MEMORYBASE_H
