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

        virtual sample process(sample const& output) {
            return {};
        };

        sample const& get_required_vars() const {
            return required_vars;
        };

        sample const& get_trigger_vars() const {
            return trigger_vars;
        };

        virtual ~IOBase() = default;
    };
}

#endif //ABTM_IOBASE_H
