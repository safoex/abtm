//
// Created by safoex on 19.11.18.
//

#ifndef BEHAVIOR_TREE_MEMORY_H
#define BEHAVIOR_TREE_MEMORY_H

#include "std_string.h"

#include <unordered_map>
#include <string>
#include <unordered_set>
#include <mutex>

#define LOG_DEBUG(x) std::cout<<x << std::endl;

typedef std::unordered_set<std::string> strset2;
namespace bt {
    template<typename T> class Memory {
        template<class K> using _memory_container = std::unordered_map<std::string, K>;
    public:
        typedef _memory_container<T> memory;
        enum Scope {
            INNER,
            OUTPUT,
            PURE_OUTPUT,
            OUTPUT_NO_SEND_ZERO
        };
        Memory();
        void set(std::string const &name, const T &value, bool need_to_lock = true);
        void set_no_output(std::string const &name, const T &value, bool need_to_lock = true);
        void add_variable(const std::string& name, Scope scope = Scope::INNER);
        void add_variables(std::initializer_list<std::string> const& names, Scope scope = Scope::INNER);
        template<class _InputIterator>
        void add_variables(_InputIterator begin, _InputIterator end, Scope scope = Scope::INNER);
        memory get_changes(Scope sc); //move ??
        void clear_changes(Scope sc);
        void set(const memory &changes);
        void set_no_output(const memory &changes);
        template<class InputIterator>
        double hash_keys(InputIterator begin, InputIterator end) const;
        template<class InputIterator>
        double hash_vals(InputIterator begin, InputIterator end) const;
        const T& operator[](std::string const& name) const;
        bool any_inner_changes() const;
        bool any_output() const;
        strset2 get_vars_start_from(const std::string& prefix) const;
        bool have(std::string const& key) const;
        memory var;
    protected:
        std::mutex lock;
        _memory_container<Scope> var_scope;
        strset2 output, inner_changes;

    };
};

#endif //BEHAVIOR_TREE_MEMORY_H
