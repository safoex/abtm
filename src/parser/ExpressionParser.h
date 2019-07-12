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


    using namespace std;

    bool delim (char c) {
        return c == ' ';
    }

    bool is_op (char c) {
        static std::set<char> ops{'+','-','*','/','>','<','=','!','&','|', '!'};
        return ops.count(c) > 0;
    }

    int priority (char op) {
        if (op < 0)
            return 6; // op == -'+' || op == -'-'
        return
                op == '&' || op == '|' ? 1 :
                op == '<' || op == '>' || op == '=' || op == '!' ? 2 :
                op == '+' || op == '-' ? 3 :
                op == '*' || op == '/' || op == '%' ? 4 :
                -1;
    }

    void process_op (vector<double> & st, char op) {
        if (op < 0) {
            double l = st.back();  st.pop_back();
            switch (-op) {
                case '+':  st.push_back (l);  break;
                case '-':  st.push_back (-l);  break;
                default:   st.push_back (l); break;
            }
        }
        else {
            double r = st.back();  st.pop_back();
            double l = st.back();  st.pop_back();
            switch (op) {
                case '+':   st.push_back (l + r);  break;
                case '-':   st.push_back (l - r);  break;
                case '*':   st.push_back (l * r);  break;
                case '/':   st.push_back (l / r);  break;
                case '>':   st.push_back (l > r);  break;
                    //case '>=':  st.push_back (l >= r);  break;
                case '<':   st.push_back (l < r);  break;
                    //case '<=':  st.push_back (l <= r);  break;
                case '=':   st.push_back (l == r);  break;
                case '!':   st.push_back (l != r); break;
                case '&':   st.push_back (l && r); break;
                case '|':   st.push_back (l || r); break;
                default:    throw std::exception(); //TODO
            }
        }
    }

    bool isunary(char op) {
        return op == '+' || op == '-';
    }

    bool isalnum_(char c) {
        return isalnum(c) || c == '_';
    }

    bool isdigit_or_dot(char c){
        return c=='.' || isdigit(c);
    }

    double calc (string const& s, const Memory<double>& m) {
        bool may_unary = false;
        vector<double> st;
        vector<char> op;
        for (size_t i=0; i<s.length(); ++i)
            if (!delim (s[i])) {
                if (s[i] == '(') {
                    op.push_back('(');
                    may_unary = true;
                } else if (s[i] == ')') {
                    while (op.back() != '(')
                        process_op(st, op.back()), op.pop_back();
                    op.pop_back();
                    may_unary = false;
                } else if (is_op(s[i])) {
                    char curop = s[i];
                    if (may_unary && isunary(curop)) curop = -curop;
                    while (!op.empty() && (
                            (curop >= 0 && priority(op.back()) >= priority(curop)) ||
                            (curop < 0 && priority(op.back()) > priority(curop)))
                            )
                        process_op(st, op.back()), op.pop_back();
                    op.push_back(curop);
                    may_unary = true;
                } else {
                    string operand;
                    while (i < s.length() && (isalnum_(s[i]) || isdigit_or_dot(s[i])))
                        operand += s[i++];
                    --i;
                    if (isdigit_or_dot(operand[0]))
                        st.push_back(strtod(operand.c_str(), nullptr));
                    else
                        st.push_back(m[operand]);
                    may_unary = false;
                }
            }
        while (!op.empty())
            process_op (st, op.back()),  op.pop_back();
        return st.back();
    }

    std::unordered_set<std::string> find_vars(std::string const& s) {
        std::unordered_set<std::string> vars;
        bool may_unary = false;
        vector<double> st;
        vector<char> op;
        for (size_t i=0; i<s.length(); ++i)
            if (!delim (s[i])) {
                if (s[i] == '(') {
                    op.push_back('(');
                    may_unary = true;
                } else if (s[i] == ')') {
                    while (op.back() != '(')
                        process_op(st, op.back()), op.pop_back();
                    op.pop_back();
                    may_unary = false;
                } else if (is_op(s[i])) {
                    char curop = s[i];
                    if (may_unary && isunary(curop)) curop = -curop;
                    while (!op.empty() && (
                            (curop >= 0 && priority(op.back()) >= priority(curop)) ||
                            (curop < 0 && priority(op.back()) > priority(curop)))
                            )
                        process_op(st, op.back()), op.pop_back();
                    op.push_back(curop);
                    may_unary = true;
                } else {
                    string operand;
                    while (i < s.length() && (isalnum_(s[i]) || isdigit_or_dot(s[i])))
                        operand += s[i++];
                    --i;
                    if (isdigit_or_dot(operand[0]))
                        st.push_back(strtod(operand.c_str(), nullptr));
                    else {
                        st.push_back(0);
                        vars.insert(operand);
                    }
                    may_unary = false;
                }
            }
        return vars;
    }

}


#endif //ABTM_EXPRESSIONPARSER_H
