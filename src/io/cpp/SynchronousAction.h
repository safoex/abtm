//
// Created by safoex on 23.07.19.
//

#ifndef ABTM_SYNCHRONOUSACTION_H
#define ABTM_SYNCHRONOUSACTION_H

#include "defs.h"
#include "../IOBase.h"
#include <functional>

namespace bt {
    class SynchronousAction : public IOBase {

    protected:
        ExternalFunction f;
    public:
        SynchronousAction(ExternalFunction const& extf, sample const& required_vars, sample const& trigger_vars);
        sample process(sample const& s) override;
        ~SynchronousAction() override = default;
    };
}

#endif //ABTM_SYNCHRONOUSACTION_H
