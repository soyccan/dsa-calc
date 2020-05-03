#ifndef _DSA_PARSER_HPP_
#define _DSA_PARSER_HPP_ 1

#include <cstdlib>
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
            // operand
            bufn = 0;
            while (__is_value(str[i]) && i < str.size()) {
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

            // operator
            Operator<T>* newop = NULL;
            Operator<T>* newop2 = NULL;
            if (str[i] == '(') {
                newop = _operator_list->LPAR;
            } else if (str[i] == ')') {
                newop = _operator_list->RPAR;
            } else if (str[i] == ',') {
                newop = _operator_list->RPAR;
                newop2 = _operator_list->LPAR;
            } else if ((i == 0 || last_op) &&
                       (str[i] == '+' || str[i] == '-')) {
                // handle ambiguous unary + and -
                if (str[i] == '+') {
                    newop = _operator_list->PLUS;
                } else {
                    newop = _operator_list->MINUS;
                }
            } else {
                for (Operator<T>* op : _operator_list->members) {
                    // op is sorted by longest repr first
                    if (!(op->operands == 1 &&
                          (op->repr == "+" || op->repr == "-")) &&
                        str.compare(i, op->repr.size(), op->repr) == 0) {
                        // skip unary + and -
                        // TODO: performance on string comparison
                        newop = op;
                        break;
                    }
                }
            }

            if (!newop) {
                throw std::invalid_argument("invalid operator");
            }
            // LOG("op: %s", % *newop);

            __expr_infix.push_back(newop);
            if (newop2)
                __expr_infix.push_back(newop2);

            last_op = newop != _operator_list->RPAR;
            i += newop->repr.size();  // NOTE: when char is ',' newop would be
                                      // ')'. They happen to be of same length
        }
        LOG("  string: %s\n  infix: %s", % str % __expr_infix);
        return __expr_infix;
    }

    const Expression<T>& infix_to_postfix()
    {
        Node<T>** final = new Node<T>*[__expr_infix.size()];
        Operator<T>** tmp = new Operator<T>*[__expr_infix.size()];
        size_t finaln = 0, tmpn = 0;

#ifdef SHOW_STACK
#define __stack_show()                \
    {                                 \
        LOGN("final:");               \
        FOR (size_t, i, 0, finaln) {  \
            LOGN("%s ", % *final[i]); \
        }                             \
        LOGN("\t\t\ttmp:");           \
        FOR (size_t, i, 0, tmpn) {    \
            LOGN("%s ", % *tmp[i]);   \
        }                             \
        LOGN("");                     \
    }
#define __stack_status3(action, stack, elem)           \
    {                                                  \
        LOG("%s-%s:%s\t", % #action % #stack % *elem); \
        __stack_show();                                \
    }
#define __stack_status2(action, stack)      \
    {                                       \
        LOG("%s-%s\t", % #action % #stack); \
        __stack_show();                     \
    }
#else
#define __stack_show()
#define __stack_status3(action, stack, elem)
#define __stack_status2(action, stack)
#endif

        for (Node<T>* node : __expr_infix) {
            // LOG("current:%s ", % *node);

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
                    while (tmpn > 0 && tmp[tmpn - 1] != _operator_list->LPAR &&
                           __is_prior(tmp[tmpn - 1], opt)) {
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
        LOG("  postfix: %s", % __expr_postfix);

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
                        "ultra-binary operator is unsupported");
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
    static inline bool __is_prior(Operator<T>* left, Operator<T>* right)
    {
        assert(left->pri >= 0);
        assert(right->pri >= 0);

        if (left->pri != right->pri) {
            return left->pri < right->pri;
        } else {
            if (left->assoc == LEFT && right->assoc == LEFT) {
                return true;
            } else if (left->assoc == RIGHT && right->assoc == RIGHT) {
                return false;
            } else {
                throw std::invalid_argument(
                    "__is_prior() unknown assiciativity");
            }
        }
    }

    static inline bool __is_value(char c)
    {
        return ('0' <= c && c <= '9') || c == '.';
    }

    Expression<T> __expr_infix, __expr_postfix;
};


/* IntParser */
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
/* end IntParser */



/* FloatParser */
template <typename T>
class FloatParser : public Parser<T>
{
public:
    FloatParser() { this->_init_operator_list(); }
    ~FloatParser() { this->_destroy_operator_list(); }

protected:
    void _init_operator_list() override
    {
        this->_operator_list = new CFloatOperatorList<T>();
    }
    void _destroy_operator_list() override { delete this->_operator_list; }

    inline T _strtoval(const char* str) override;
    inline T _strtoval(std::string str) override;
};

template <>
inline double FloatParser<double>::_strtoval(const char* str)
{
    return std::atof(str);
}

template <>
inline double FloatParser<double>::_strtoval(std::string str)
{
    return std::stod(str);
}
/* end FloatParser */

#endif