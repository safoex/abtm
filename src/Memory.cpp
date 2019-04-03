//
// Created by safoex on 19.11.18.
//
//#pragma once

#include "Memory.h"
#include "Node.h"
#include "Tree.h"

#define LOGGING false



namespace bt {
    template<typename T> Memory<T>::Memory(): var(), var_scope(), output() {}

    template<typename T> void Memory<T>::set(std::string const &name, const T &value, bool need_to_lock) {
//        if(name == "time")
//            LOG_DEBUG("entered set");
        if(need_to_lock)
            lock.lock();
//        if(name == "time")
//            LOG_DEBUG("time set");
        if(!var_scope.count(name))
            var_scope[name] = Scope ::PURE_OUTPUT;

        Scope sc = var_scope[name];

        if( (sc == Scope::OUTPUT && var[name] != value) || sc == PURE_OUTPUT || (sc == OUTPUT_NO_SEND_ZERO && value != 0)) {
            output.insert(name);
        }

        var[name] = value;

        if(sc == Scope::INNER) {
            inner_changes.insert(name);
        }

        if(LOGGING)
            LOG_DEBUG("ar_bt_test: memory set " + name + " : " + std::to_string(value) );

        if(need_to_lock)
            lock.unlock();
    }

    template<typename T> void Memory<T>::set_no_output(std::string const &name, const T &value, bool need_to_lock) {
        if(need_to_lock)
            lock.lock();
        if(!var_scope.count(name))
            var_scope[name] = Scope ::PURE_OUTPUT;

        Scope sc = var_scope[name];

        //no addings to output

        var[name] = value;

        if(sc == Scope::INNER) {
            inner_changes.insert(name);
        }

        if(LOGGING)
            LOG_DEBUG("ar_bt_test: memory set " + name + " : " + std::to_string(value) );

        if(need_to_lock)
            lock.unlock();
    }



    template<typename T> void Memory<T>::set(const bt::Memory<T>::memory &changes) {
        lock.lock();
        for(auto const &p: changes) {
            set(p.first, p.second, false);
        }
        lock.unlock();
    }

    template<typename T> void Memory<T>::set_no_output(const bt::Memory<T>::memory &changes) {
        lock.lock();
        for(auto const &p: changes) {
            set_no_output(p.first, p.second, false);
        }
        lock.unlock();
    }

    template <typename T> const T& Memory<T>::operator[](std::string const& name) const {
        //DEBUG_PR("memory []: " + name + " " + std::to_string(var.at(name)));
        return var.at(name);
    }

    template <typename T> strset2 Memory<T>::get_vars_start_from(const std::string &prefix) const {
        //TODO: inefficient implemetation
        strset2 res;
        std::string a;
        for(const auto& p: var) {
            if(p.first.compare(0,prefix.length(),prefix) == 0) res.insert(p.first);
        }
        return res;
    }

    template <typename T> void Memory<T>::add_variable(const std::string &name, bt::Memory<T>::Scope scope) {
        lock.lock();
        if(!var.count(name)) {
            var[name] = 0;
            var_scope[name] = scope;
        }
        lock.unlock();
    }

    template <typename T> typename Memory<T>::memory Memory<T>::get_changes(bt::Memory<T>::Scope sc)  {
        lock.lock();
        bt::Tree::dict<double> c;
        if(sc == bt::Memory<T>::Scope::INNER) {
            for(const auto&v : inner_changes){
                try{
                    c[v] = var.at(v);
                }
                catch(...) {
                    DEBUG_PR("keyerror in get_changes");
                }
            }
        }
        else if (sc == bt::Memory<T>::Scope::OUTPUT || sc == bt::Memory<T>::Scope::PURE_OUTPUT){

            for(const auto& v: output) {
                try {
                    c[v] = var.at(v);
                }
                catch(...) {
                    DEBUG_PR("keyerror in get_changes");
                }

            }
        };
        lock.unlock();
        return c;
    }

    template <typename T> void Memory<T>::clear_changes(bt::Memory<T>::Scope sc) {
        lock.lock();
        if(sc == bt::Memory<T>::Scope::INNER)
            inner_changes.clear();
        else if (sc == bt::Memory<T>::Scope::OUTPUT || sc == bt::Memory<T>::Scope::PURE_OUTPUT){
            output.clear();
        }
        lock.unlock();
    }


    template <typename T> bool Memory<T>::any_inner_changes() const {
        return inner_changes.size() > 0;
    }

    template <typename T> bool Memory<T>::any_output() const {
        return output.size() > 0;
    }

    template<typename T>
    template<class InputIterator>
    double Memory<T>::hash_vals(InputIterator begin, InputIterator end) const {
        std::string big_str;
        for(auto it = begin; it != end; it++) {
            big_str += std::to_string(*it);
        }
        return std::hash<std::string>()(big_str);
    }

    template<typename T>
    template<class InputIterator>
    double Memory<T>::hash_keys(InputIterator begin, InputIterator end) const {
        std::string big_str;
        for(auto it = begin; it != end; it++) {
            big_str += std::to_string(var.at(*it));
        }
        return std::hash<std::string>()(big_str);
    }
    template<typename T>
    template<class _InputIterator>
    void Memory<T>::add_variables(_InputIterator begin, _InputIterator end, bt::Memory<T>::Scope scope) {
        for(auto it = begin; it != end; it++)
            add_variable(*it, scope);
    }

    template <typename T>
    void Memory<T>::add_variables(std::initializer_list<std::string> const &names, bt::Memory<T>::Scope scope) {
        for(auto const& n: names)
            add_variable(n, scope);
    }

    template<typename T>
    bool Memory<T>::have(std::string const &key) const {
        return var.count(key) > 0;
    }
};
