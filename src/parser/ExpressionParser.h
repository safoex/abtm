//
// Created by safoex on 09.07.19.
//

#ifndef ABTM_EXPRESSIONPARSER_H
#define ABTM_EXPRESSIONPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "BaseParser.h"

namespace bt {
    class ExpressionParser {
        typedef std::function<double(const Memory<double> &)> RValueFunction;
    protected:
        Memory<double> &m;

    public:
        explicit ExpressionParser(Memory<double> &memory);

        Action::ActionaryFunction get_actionary_function(std::string const &lvalue, std::string const &rvalue) const;

        Condition::ConditionaryFunction get_conditionary_function(std::string const &success = "default",
                                                                  std::string const &failed = "__never",
                                                                  std::string const &running = "__never") const;

        RValueFunction get_rvalue_function(std::string const &rvalue) const;

    };

    namespace expr {

        bool delim (char c);

        bool is_op (char c);

        int priority (char op);

        void process_op (std::vector<double> & st, char op);

        bool isunary(char op);

        bool isalnum_(char c);

        bool isdigit_or_dot(char c);

        double calc (std::string const& s, const Memory<double>& m);

        std::unordered_set<std::string> find_vars(std::string const& s);
    }
}



#endif //ABTM_EXPRESSIONPARSER_H
