#ifndef _DSA_PARSER_HPP_
#define _DSA_PARSER_HPP_ 1

#include <cstdlib>
#include <stdexcept>
#include <string>

#include "expression.hpp"
#include "operatorlist.hpp"
#include "tools.h"

template <typename T>
class Parser
{
public:
    Parser() {}
    ~Parser()
    {
        __expr_infix.clear();
        __expr_postfix.clear();
    }

    const Expression<T>& parse_infix(std::string str)
    {
        // assume str contains no space
        // little error handling is implemented
        __expr_infix.clear();
        char buf[32];  // enough for int64
        size_t bufn = 0;
        bool last_op = false;  // whether last one pushed into __expression is
                               // non-right-parenthesis operator
        size_t i = 0;
        while (i < str.size()) {
            // operand
            bufn = 0;
            while (i < str.size() && __is_value(str[i])) {
                buf[bufn++] = str[i++];
                if (bufn >= sizeof(buf))
                    throw std::invalid_argument("operand too large");
            }
            if (bufn > 0) {
                buf[bufn] = '\0';
                // TODO: is there better way than using shared pointer?
                __expr_infix.push_back(
                    std::make_shared<const Operand<T>>(_strtoval(buf)));
                last_op = false;
            }

            if (i == str.size())
                break;

            // operator
            std::shared_ptr<const Operator<T>> newop;
            std::shared_ptr<const Operator<T>> newop2;
            if (str[i] == ',') {
                // e.g. pow(2,4) --> pow(2)(4)
                newop = _operator_list.RPAR;
                newop2 = _operator_list.LPAR;
                last_op = true;
            } else if (i == 0 || last_op) {
                // handle ambiguous unary + and -
                if (str[i] == '+') {
                    newop = _operator_list.PLUS;
                } else if (str[i] == '-') {
                    newop = _operator_list.MINUS;
                }
                last_op = true;
            }

            if (!newop) {
                for (std::shared_ptr<const Operator<T>> op : _operator_list) {
                    // op is sorted by longest repr first
                    if (!(op->operands() == 1 &&
                          (op->repr() == "+" || op->repr() == "-")) &&
                        str.compare(i, op->repr().size(), op->repr()) == 0) {
                        // skip unary + and -
                        // TODO: performance on string comparison
                        newop = op;
                        last_op = op != _operator_list.RPAR;
                        break;
                    }
                }
            }

            if (!newop) {
                throw std::invalid_argument("invalid operator");
            }

            __expr_infix.push_back(newop);
            if (newop2)
                __expr_infix.push_back(newop2);

            i += newop->repr().size();  // NOTE: when char is ',' newop would be
                                        // ')'. They happen to be of same length
        }
        LOG("  string: %s\n  infix: %s", % str % __expr_infix);
        return __expr_infix;
    }

    const Expression<T>& infix_to_postfix()
    {
        auto final =
            std::make_unique<std::shared_ptr<const Node<T>>[]>(__expr_infix.size());
        auto tmp =
            std::make_unique<std::shared_ptr<const Operator<T>>[]>(__expr_infix.size());

        size_t finaln = 0, tmpn = 0;

#ifdef SHOW_STACK
#define __stack_show()                \
    {                                 \
        LOGN("final:");               \
        FOR (size_t, i, 0, finaln) {  \
            LOGN("%s ", % *final[i]); \
        }                             \
        LOGN("\t\t\t\t\t\t\t\ttmp:"); \
        FOR (size_t, i, 0, tmpn) {    \
            LOGN("%s ", % *tmp[i]);   \
        }                             \
        LOG("");                      \
    }
#define __stack_status3(action, stack, elem) \
    {                                        \
        __stack_show();                      \
    }
#define __stack_status2(action, stack) \
    {                                  \
        __stack_show();                \
    }
#else
#define __stack_show()
#define __stack_status3(action, stack, elem)
#define __stack_status2(action, stack)
#endif

        for (const std::shared_ptr<const Node<T>>& node : __expr_infix) {
            // LOG("current:%s ", % *node);

            auto opt = std::dynamic_pointer_cast<const Operator<T>>(node);
            auto opd = std::dynamic_pointer_cast<const Operand<T>>(node);
            assert((opt == NULL) ^ (opd == NULL));

            if (opt != NULL) {
                if (opt == _operator_list.LPAR) {
                    tmp[tmpn++] = opt;
                    __stack_status3(push, tmp, opt);
                } else if (opt == _operator_list.RPAR) {
                    while (tmpn > 0 && tmp[tmpn - 1] != _operator_list.LPAR) {
                        final[finaln++] = tmp[tmpn - 1];
                        tmpn--;
                        __stack_status3(move, tmp_to_final, tmp[tmpn]);
                    }
                    assert(tmpn > 0);
                    tmpn--;
                    __stack_status2(pop, tmp);
                } else {
                    while (tmpn > 0 && tmp[tmpn - 1] != _operator_list.LPAR &&
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

        return __expr_postfix;
    }

    T calculate()
    {
        auto st = std::make_unique<T[]>(__expr_postfix.size());  // stack
        size_t stn = 0;

        for (const std::shared_ptr<const Node<T>>& node : __expr_postfix) {
            auto opt = std::dynamic_pointer_cast<const Operator<T>>(node);
            auto opd = std::dynamic_pointer_cast<const Operand<T>>(node);
            assert((opt == NULL) ^ (opd == NULL));

            if (opt != NULL) {
                if (opt->operands() == 1) {
                    assert(stn >= 1);
                    st[stn - 1] = opt->apply()(st[stn - 1], 0);
                } else if (opt->operands() == 2) {
                    assert(stn >= 2);
                    st[stn - 2] = opt->apply()(st[stn - 2], st[stn - 1]);
                    stn--;
                } else {
                    throw std::invalid_argument("ultra-binary operator is unsupported");
                }
            } else {
                assert(opd != NULL);
                st[stn++] = opd->val;
            }
        }

        assert(stn == 1);
        T res = st[0];
        return res;
    }

protected:
    virtual void _init_operator_list() = 0;
    virtual void _destroy_operator_list() {}
    virtual inline T _strtoval(const char* str) = 0;
    virtual inline T _strtoval(std::string str) = 0;

    OperatorList<T> _operator_list;

private:
    static inline bool __is_prior(const std::shared_ptr<const Operator<T>>& left,
                                  const std::shared_ptr<const Operator<T>>& right)
    {
        assert(left->pri() >= 0);
        assert(right->pri() >= 0);

        if (left->pri() != right->pri()) {
            return left->pri() < right->pri();
        } else {
            if (left->assoc() == Operator<T>::LEFT &&
                right->assoc() == Operator<T>::LEFT) {
                return true;
            } else if (left->assoc() == Operator<T>::RIGHT &&
                       right->assoc() == Operator<T>::RIGHT) {
                return false;
            } else {
                throw std::invalid_argument("__is_prior() unknown assiciativity");
            }
        }
    }

    static inline bool __is_value(char c) { return ('0' <= c && c <= '9') || c == '.'; }

    Expression<T> __expr_infix, __expr_postfix;
};


/* IntParser */
template <typename T>
class IntParser : public Parser<T>
{
public:
    IntParser() { this->_init_operator_list(); }

protected:
    void _init_operator_list() override { this->_operator_list = CIntOperatorList<T>(); }

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

protected:
    void _init_operator_list() override
    {
        this->_operator_list = CFloatOperatorList<T>();
    }

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