//
// Created by safoex on 25.07.19.
//

#ifndef ABTM_CPPFUNCTIONPARSER_H
#define ABTM_CPPFUNCTIONPARSER_H

#include "defs.h"
#include "SynchronousAction.h"
#include "../../parser/BaseParser.h"

namespace bt {
    class CPPFunctionParser : public BaseParser {
        dictOf <ExternalFunction> functions;
        std::vector<IOBase*> io_modules;

    public:
        CPPFunctionParser(Builder& builder);

        void insert(std::string const& name, ExternalFunction const& function);
        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        std::string get_var_name(std::string const& function_name, std::string const& var_type) const;
        ~CPPFunctionParser() override;
    };
}

#endif //ABTM_CPPFUNCTIONPARSER_H
