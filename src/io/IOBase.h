//
// Created by safoex on 23.07.19.
//

#ifndef ABTM_IOBASE_H
#define ABTM_IOBASE_H

#include "ExecutorBase.h"
namespace bt {
    class IOBase {
    protected:
        sample required_vars, trigger_vars;
    public:
        IOBase(sample const& required_vars, sample const& trigger_vars)
            : required_vars(required_vars), trigger_vars(trigger_vars) {};

        IOBase() {}

        void set_vars(sample const& _required_vars, sample const& _trigger_vars) {
            required_vars = _required_vars;
            trigger_vars = _trigger_vars;
        }
        virtual sample process(sample const& output) {
            return {};
        };

        virtual sample const& get_required_vars() const {
            return required_vars;
        };

        virtual sample const& get_trigger_vars() const {
            return trigger_vars;
        };

        virtual ~IOBase() = default;
    };
}

#endif //ABTM_IOBASE_H
