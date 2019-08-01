//
// Created by safoex on 26.07.19.
//

#ifndef ABTM_CPPFUNCTION_H
#define ABTM_CPPFUNCTION_H

#include <yaml-cpp/node/node.h>
#include "defs.h"

namespace bt {
    class CPPFunction {
    public:
        ExternalFunction f;
        sample required_vars;
        std::string name;
        unsigned priority;
        CPPFunction(ExternalFunction f = {}, std::string name = "", sample required_vars = {}, unsigned priority = 0);

        std::string get_yaml_recipe() const;
        YAML::Node get_YAML_recipe() const;
    };
}

#endif //ABTM_CPPFUNCTION_H
