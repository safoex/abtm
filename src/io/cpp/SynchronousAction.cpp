//
// Created by safoex on 23.07.19.
//

#include "SynchronousAction.h"

namespace bt {
    SynchronousAction::SynchronousAction(bt::ExternalFunction const& extf, bt::sample const& required_vars,
            bt::sample const& trigger_vars) : IOBase(required_vars, trigger_vars), f(extf) {
    }

    sample SynchronousAction::process(const bt::sample &s) {
        return f(s);
    }
}