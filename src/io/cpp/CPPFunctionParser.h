//
// Created by safoex on 25.07.19.
//

#ifndef ABTM_CPPFUNCTIONPARSER_H
#define ABTM_CPPFUNCTIONPARSER_H

#include "defs.h"
#include "SynchronousAction.h"
#include "CPPFunction.h"
#include "../IOParser.h"

namespace bt {
    class CPPFunctionParser : public IOParser {
        dictOf <CPPFunction> functions;
    public:
        CPPFunctionParser(Builder& builder, MIMOCenter& mimo);
        CPPFunctionParser(Builder& builder, MIMOCenter& mimo, std::vector<CPPFunction> const& args);
        CPPFunctionParser(Builder& builder, MIMOCenter& mimo, std::vector<std::vector<CPPFunction>> const& args);


        void insert(CPPFunction const& cppFunction);
        void insert(std::vector<std::vector<CPPFunction>> const& cppFunctions);
        void insert(std::vector<CPPFunction> const& cppFunctions);

        void parse_one(std::string const& id, YAML::Node const &yaml_node);
        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        std::string get_var_name(std::string const& function_name, std::string const& var_type) const;
    };
}

#endif //ABTM_CPPFUNCTIONPARSER_H
