#ifndef _DSA_PARSER_H_
#define _DSA_PARSER_H_ 1

#include <stdexcept>
#include <string>

#include "operatorlist.hpp"
#include "tools.h"

template <typename T>
class Parser
{
public:
    Parser() {}

    const Expression<T>& parse_infix(std::string str)
    {
        // assume str contains no space
        __expr_infix.clear();
        char buf[32];  // enough for int64
        int bufn = 0;
        bool last_op = false;  // whether last one pushed into __expression is
                               // non-right-parenthesis operator
        size_t i = 0;
        while (i < str.size()) {
            bufn = 0;
            while (isdigit(str[i]) && i < str.size()) {
                buf[bufn++] = str[i];
                i++;
            }
            buf[bufn] = '\0';
            if (bufn > 0) {
                __expr_infix.push_back(new Operand<T>(_strtoval(buf)));
                last_op = false;
                // LOG("push value: %s") % *__expr_infix.back();
            }

            if (i == str.size())
                break;

            Operator<T>* newop = NULL;
            if (str[i] == '(') {
                newop = _operator_list->LPAR;
                last_op = true;
            } else if (str[i] == ')') {
                newop = _operator_list->RPAR;
            } else if (i == 0 || last_op) {
                // unary
                if (str[i] == '+') {
                    newop = _operator_list->PLUS;
                } else if (str[i] == '-') {
                    newop = _operator_list->MINUS;
                } else if (str[i] == '!') {
                    newop = _operator_list->LNOT;
                } else if (str[i] == '~') {
                    newop = _operator_list->NOT;
                } else {
                    throw std::invalid_argument("invalid unary operator");
                }
                // LOG("push unary: %s") % *newop;
                last_op = true;
            } else {
                // binary
                for (Operator<T>* op : _operator_list->members) {
                    if (op->operands == 2 &&
                        str.compare(i, op->repr.size(), op->repr) == 0) {
                        // TODO:performance
                        assert(newop == NULL);
                        newop = op;
                        last_op = true;
                        // LOG("push binary: %s") % *newop;
                    }
                }
            }
            assert(newop != NULL);
            __expr_infix.push_back(newop);
            i += newop->repr.size();
        }
        LOG("string:%s infix:%s") % str % __expr_infix;
        return __expr_infix;
    }

    const Expression<T>& infix_to_postfix()
    {
        Node<T>** final = new Node<T>*[__expr_infix.size()];
        Operator<T>** tmp = new Operator<T>*[__expr_infix.size()];
        size_t finaln = 0, tmpn = 0;

#define __stack_show()                     \
    {                                      \
        std::cerr << "final:";             \
        FOR (size_t, i, 0, finaln) {       \
            std::cerr << *final[i] << ' '; \
        }                                  \
        std::cerr << "\t\t\ttmp:";         \
        FOR (size_t, i, 0, tmpn) {         \
            std::cerr << *tmp[i] << ' ';   \
        }                                  \
        std::cerr << "\n";                 \
    }
#define __stack_status3(action, stack, elem)                                 \
    {                                                                        \
        std::cerr << boost::format("%s-%s:%s\t") % #action % #stack % *elem; \
        __stack_show();                                                      \
    }
#define __stack_status2(action, stack)                            \
    {                                                             \
        std::cerr << boost::format("%s-%s\t") % #action % #stack; \
        __stack_show();                                           \
    }

        for (Node<T>* node : __expr_infix) {
            std::cerr << boost::format("current:%s ") % *node;

            Operator<T>* opt = dynamic_cast<Operator<T>*>(node);
            Operand<T>* opd = dynamic_cast<Operand<T>*>(node);
            assert((opt == NULL) ^ (opd == NULL));

            if (opt != NULL) {
                if (opt == _operator_list->LPAR) {
                    tmp[tmpn++] = opt;
                    __stack_status3(push, tmp, opt);
                } else if (opt == _operator_list->RPAR) {
                    while (tmpn > 0 && tmp[tmpn - 1] != _operator_list->LPAR) {
                        final[finaln++] = tmp[tmpn - 1];
                        tmpn--;
                        __stack_status3(move, tmp_to_final, tmp[tmpn]);
                    }
                    assert(tmpn > 0);
                    tmpn--;
                    __stack_status2(pop, tmp);
                } else {
                    while (tmpn > 0 && __is_prior(tmp[tmpn - 1], opt)) {
                        final[finaln++] = tmp[tmpn - 1];
                        tmpn--;
                        __stack_status3(move, tmp_to_final, tmp[tmpn]);
                    }
                    tmp[tmpn++] = opt;
                    __stack_status3(push, tmp, opt);
                }
            } else {
                assert(opd != NULL);
                final[finaln++] = opd;
                __stack_status3(push, final, opd);
            }
        }
        while (tmpn > 0) {
            final[finaln++] = tmp[tmpn - 1];
            tmpn--;
            __stack_status3(move, tmp_to_final, tmp[tmpn]);
        }
#undef __stack_status2
#undef __stack_status3
#undef __stack_show

        assert(tmpn == 0);
        __expr_postfix.clear();
        FOR (size_t, i, 0, finaln) {
            __expr_postfix.push_back(final[i]);
        }
        LOG("postfix: %s") % __expr_postfix;

        delete[] final;
        delete[] tmp;
        return __expr_postfix;
    }

    T calculate()
    {
        T* st = new T[__expr_postfix.size()];  // stack
        size_t stn = 0;

        for (Node<T>* node : __expr_postfix) {
            Operator<T>* opt = dynamic_cast<Operator<T>*>(node);
            Operand<T>* opd = dynamic_cast<Operand<T>*>(node);
            assert((opt == NULL) ^ (opd == NULL));

            if (opt != NULL) {
                if (opt->operands == 1) {
                    assert(stn >= 1);
                    st[stn - 1] = opt->apply(st[stn - 1], 0);
                } else if (opt->operands == 2) {
                    assert(stn >= 2);
                    st[stn - 2] = opt->apply(st[stn - 2], st[stn - 1]);
                    stn--;
                } else {
                    throw std::invalid_argument(
                        "trinary operator is unsupported");
                }
            } else {
                assert(opd != NULL);
                st[stn++] = opd->val;
                delete opd;
            }
        }

        assert(stn == 1);
        T res = st[0];
        delete[] st;
        return res;
    }

protected:
    virtual void _init_operator_list() = 0;
    virtual void _destroy_operator_list() = 0;
    virtual inline T _strtoval(const char* str) = 0;
    virtual inline T _strtoval(std::string str) = 0;

    OperatorList<T>* _operator_list;

private:
    inline bool __is_prior(Operator<T>* left, Operator<T>* right)
    {
        assert(right->pri >= 0);
        if (left == _operator_list->LPAR) {
            return false;
        }
        assert(left->pri >= 0);

        if (left->pri < right->pri) {
            return true;
        } else if (left->pri == right->pri) {
            if (left->l_assoc && right->l_assoc) {
                return true;
            } else {
                assert(!left->l_assoc && !right->l_assoc);
                return false;
            }
        } else {
            return false;
        }
    }

    Expression<T> __expr_infix, __expr_postfix;
};


template <typename T>
class IntParser : public Parser<T>
{
public:
    IntParser() { this->_init_operator_list(); }
    ~IntParser() { this->_destroy_operator_list(); }

protected:
    void _init_operator_list() override
    {
        this->_operator_list = new CIntOperatorList<T>();
    }
    void _destroy_operator_list() override { delete this->_operator_list; }

    inline T _strtoval(const char* str) override;
    inline T _strtoval(std::string str) override;
};


template <>
inline int IntParser<int>::_strtoval(const char* str)
{
    return std::atoi(str);
}

template <>
inline int IntParser<int>::_strtoval(std::string str)
{
    return std::stoi(str);
}

#endif