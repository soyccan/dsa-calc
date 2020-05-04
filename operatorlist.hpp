#ifndef _DSA_OPERATORLIST_HPP_
#define _DSA_OPERATORLIST_HPP_ 1

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <vector>

#include "node.hpp"


template <typename T>
class OperatorList
{
public:
    OperatorList() {}

    typename std::vector<std::shared_ptr<const Operator<T>>>::const_iterator begin() const
    {
        return _members.cbegin();
    }
    typename std::vector<std::shared_ptr<const Operator<T>>>::const_iterator end() const
    {
        return _members.cend();
    }

    // common operators
    std::shared_ptr<const Operator<T>> MUL;
    std::shared_ptr<const Operator<T>> DIV;
    std::shared_ptr<const Operator<T>> ADD;
    std::shared_ptr<const Operator<T>> SUB;
    std::shared_ptr<const Operator<T>> LPAR;
    std::shared_ptr<const Operator<T>> RPAR;
    std::shared_ptr<const Operator<T>> EQ;
    std::shared_ptr<const Operator<T>> NEQ;
    std::shared_ptr<const Operator<T>> MINUS;
    std::shared_ptr<const Operator<T>> PLUS;


protected:
    virtual void _init_operators() {}
    virtual void _destroy_operators() {
    }  // this may not be used as long as we use shared_ptr

    std::vector<std::shared_ptr<const Operator<T>>> _members;
};


template <typename T>
class CIntOperatorList : public OperatorList<T>
{
public:
    CIntOperatorList() { this->_init_operators(); }

    // integer specific operators
    std::shared_ptr<const Operator<T>> MOD;
    std::shared_ptr<const Operator<T>> AND;
    std::shared_ptr<const Operator<T>> XOR;
    std::shared_ptr<const Operator<T>> OR;
    std::shared_ptr<const Operator<T>> SHL;
    std::shared_ptr<const Operator<T>> SHR;
    std::shared_ptr<const Operator<T>> LAND;
    std::shared_ptr<const Operator<T>> LOR;
    std::shared_ptr<const Operator<T>> NOT;
    std::shared_ptr<const Operator<T>> LNOT;


protected:
    void _init_operators() override
    {
        // https://en.cppreference.com/w/c/language/operator_precedence
        int p;
        this->_members.clear();

        p = 2;
        this->_members.push_back(
            this->MINUS = std::make_shared<Operator<T>>(p, "-", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->PLUS = std::make_shared<Operator<T>>(p, "+", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->NOT = std::make_shared<Operator<T>>(p, "~", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->LNOT = std::make_shared<Operator<T>>(p, "!", 1, Operator<T>::RIGHT));

        p = 3;
        this->_members.push_back(this->MUL = std::make_shared<Operator<T>>(p, "*"));
        this->_members.push_back(this->DIV = std::make_shared<Operator<T>>(p, "/"));
        this->_members.push_back(this->MOD = std::make_shared<Operator<T>>(p, "%"));

        p = 4;
        this->_members.push_back(this->ADD = std::make_shared<Operator<T>>(p, "+"));
        this->_members.push_back(this->SUB = std::make_shared<Operator<T>>(p, "-"));

        p = 5;
        this->_members.push_back(this->SHL = std::make_shared<Operator<T>>(p, "<<"));
        this->_members.push_back(this->SHR = std::make_shared<Operator<T>>(p, ">>"));

        p = 7;
        this->_members.push_back(this->EQ = std::make_shared<Operator<T>>(p, "=="));
        this->_members.push_back(this->NEQ = std::make_shared<Operator<T>>(p, "!="));

        p = 8;
        this->_members.push_back(this->AND = std::make_shared<Operator<T>>(p, "&"));

        p = 9;
        this->_members.push_back(this->XOR = std::make_shared<Operator<T>>(p, "^"));

        p = 10;
        this->_members.push_back(this->OR = std::make_shared<Operator<T>>(p, "|"));

        p = 11;
        this->_members.push_back(this->LAND = std::make_shared<Operator<T>>(p, "&&"));

        p = 12;
        this->_members.push_back(this->LOR = std::make_shared<Operator<T>>(p, "||"));

        // special operators, not in postfix
        p = -1;
        this->_members.push_back(this->LPAR = std::make_shared<Operator<T>>(p, "("));
        this->_members.push_back(this->RPAR = std::make_shared<Operator<T>>(p, ")"));

        // define functions
        std::const_pointer_cast<Operator<T>>(this->MINUS)->apply([](T x, T _) {
            return -x;
        });
        std::const_pointer_cast<Operator<T>>(this->PLUS)->apply([](T x, T _) {
            return x;
        });
        std::const_pointer_cast<Operator<T>>(this->NOT)->apply(
            [](T x, T _) { return ~x; });
        std::const_pointer_cast<Operator<T>>(this->LNOT)->apply([](T x, T _) {
            return !x;
        });
        std::const_pointer_cast<Operator<T>>(this->MUL)->apply(
            [](T x, T y) { return x * y; });
        std::const_pointer_cast<Operator<T>>(this->DIV)->apply(
            [](T x, T y) { return x / y; });
        std::const_pointer_cast<Operator<T>>(this->MOD)->apply(
            [](T x, T y) { return x % y; });
        std::const_pointer_cast<Operator<T>>(this->ADD)->apply(
            [](T x, T y) { return x + y; });
        std::const_pointer_cast<Operator<T>>(this->SUB)->apply(
            [](T x, T y) { return x - y; });
        std::const_pointer_cast<Operator<T>>(this->SHL)->apply(
            [](T x, T y) { return x << y; });
        std::const_pointer_cast<Operator<T>>(this->SHR)->apply(
            [](T x, T y) { return x >> y; });
        std::const_pointer_cast<Operator<T>>(this->EQ)->apply(
            [](T x, T y) { return x == y; });
        std::const_pointer_cast<Operator<T>>(this->NEQ)->apply(
            [](T x, T y) { return x != y; });
        std::const_pointer_cast<Operator<T>>(this->AND)->apply(
            [](T x, T y) { return x & y; });
        std::const_pointer_cast<Operator<T>>(this->XOR)->apply(
            [](T x, T y) { return x ^ y; });
        std::const_pointer_cast<Operator<T>>(this->OR)->apply(
            [](T x, T y) { return x | y; });
        std::const_pointer_cast<Operator<T>>(this->LAND)->apply([](T x, T y) {
            return x && y;
        });
        std::const_pointer_cast<Operator<T>>(this->LOR)->apply(
            [](T x, T y) { return x || y; });

        // longer expr first
        std::sort(this->_members.begin(), this->_members.end(),
                  [](const std::shared_ptr<const Operator<T>>& x,
                     const std::shared_ptr<const Operator<T>>& y) {
                      return x->repr().size() > y->repr().size();
                  });
    }
};


template <typename T>
class CFloatOperatorList : public OperatorList<T>
{
public:
    CFloatOperatorList() { this->_init_operators(); }

    // floating-point number specific operators
    std::shared_ptr<const Operator<T>> SIN;
    std::shared_ptr<const Operator<T>> COS;
    std::shared_ptr<const Operator<T>> EXP;
    std::shared_ptr<const Operator<T>> LOG;
    std::shared_ptr<const Operator<T>> POW;
    std::shared_ptr<const Operator<T>> SQRT;
    std::shared_ptr<const Operator<T>> ABS;


protected:
    void _init_operators() override
    {
        // https://en.cppreference.com/w/c/language/operator_precedence
        int p;
        this->_members.clear();

        p = 0;  // functions has highest priority
        this->_members.push_back(
            this->SIN = std::make_shared<Operator<T>>(p, "sin", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->COS = std::make_shared<Operator<T>>(p, "cos", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->EXP = std::make_shared<Operator<T>>(p, "exp", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->LOG = std::make_shared<Operator<T>>(p, "log", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->POW = std::make_shared<Operator<T>>(p, "pow", 2, Operator<T>::RIGHT));
        this->_members.push_back(
            this->SQRT = std::make_shared<Operator<T>>(p, "sqrt", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->ABS = std::make_shared<Operator<T>>(p, "fabs", 1, Operator<T>::RIGHT));

        p = 2;
        this->_members.push_back(
            this->MINUS = std::make_shared<Operator<T>>(p, "-", 1, Operator<T>::RIGHT));
        this->_members.push_back(
            this->PLUS = std::make_shared<Operator<T>>(p, "+", 1, Operator<T>::RIGHT));

        p = 3;
        this->_members.push_back(this->MUL = std::make_shared<Operator<T>>(p, "*"));

        p = 4;
        this->_members.push_back(this->ADD = std::make_shared<Operator<T>>(p, "+"));
        this->_members.push_back(this->SUB = std::make_shared<Operator<T>>(p, "-"));

        // special operators, not in postfix
        p = -1;
        this->_members.push_back(this->LPAR = std::make_shared<Operator<T>>(p, "("));
        this->_members.push_back(this->RPAR = std::make_shared<Operator<T>>(p, ")"));


        // define functions
        std::const_pointer_cast<Operator<T>>(this->SIN)->apply(
            [](T x, T _) { return std::sin(x); });
        std::const_pointer_cast<Operator<T>>(this->COS)->apply(
            [](T x, T _) { return std::cos(x); });
        std::const_pointer_cast<Operator<T>>(this->EXP)->apply(
            [](T x, T _) { return std::exp(x); });
        std::const_pointer_cast<Operator<T>>(this->LOG)->apply(
            [](T x, T _) { return std::log(x); });
        std::const_pointer_cast<Operator<T>>(this->POW)->apply(
            [](T x, T y) { return std::pow(x, y); });
        std::const_pointer_cast<Operator<T>>(this->SQRT)->apply([](T x, T _) {
            return std::sqrt(x);
        });
        std::const_pointer_cast<Operator<T>>(this->ABS)->apply(
            [](T x, T _) { return std::fabs(x); });
        std::const_pointer_cast<Operator<T>>(this->MINUS)->apply([](T x, T _) {
            return -x;
        });
        std::const_pointer_cast<Operator<T>>(this->PLUS)->apply([](T x, T _) {
            return x;
        });
        std::const_pointer_cast<Operator<T>>(this->MUL)->apply(
            [](T x, T y) { return x * y; });
        std::const_pointer_cast<Operator<T>>(this->ADD)->apply(
            [](T x, T y) { return x + y; });
        std::const_pointer_cast<Operator<T>>(this->SUB)->apply(
            [](T x, T y) { return x - y; });

        // longer repr first
        std::sort(this->_members.begin(), this->_members.end(),
                  [](const std::shared_ptr<const Operator<T>>& x,
                     const std::shared_ptr<const Operator<T>>& y) {
                      return x->repr().size() > y->repr().size();
                  });
    }
};

#endif