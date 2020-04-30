#ifndef _DSA_OPERATORLIST_H_
#define _DSA_OPERATORLIST_H_ 1

#include <functional>
#include <vector>

#include "expression.hpp"


template <typename T>
class OperatorList
{
public:
    OperatorList() {}
    virtual ~OperatorList() {}

    std::vector<Operator<T>*> members;

    Operator<T>* MUL;
    Operator<T>* DIV;
    Operator<T>* MOD;
    Operator<T>* ADD;
    Operator<T>* SUB;
    Operator<T>* AND;
    Operator<T>* XOR;
    Operator<T>* OR;
    Operator<T>* SHL;
    Operator<T>* SHR;
    Operator<T>* LPAR;
    Operator<T>* RPAR;
    Operator<T>* LAND;
    Operator<T>* LOR;
    Operator<T>* NOT;
    Operator<T>* LNOT;
    Operator<T>* EQ;
    Operator<T>* NEQ;
    Operator<T>* MINUS;
    Operator<T>* PLUS;

protected:
    virtual void _init_operators() = 0;
    virtual void _destroy_operators() = 0;
};


template <typename T>
class CIntOperatorList : public OperatorList<T>
{
public:
    CIntOperatorList() { this->_init_operators(); }
    ~CIntOperatorList() { this->_destroy_operators(); }

protected:
    void _init_operators() override
    {
        // https://en.cppreference.com/w/c/language/operator_precedence

        // #define __operator(name, ...)                    \
//     {                                            \
//         this->members.emplace_back(__VA_ARGS__); \
//         this->name = &(this->members.back());    \
//     }

        int p;
        p = 2;
        this->MINUS = new Operator<T>(p, "-", 1, false);
        this->PLUS = new Operator<T>(p, "+", 1, false);
        this->NOT = new Operator<T>(p, "~", 1, false);
        this->LNOT = new Operator<T>(p, "!", 1, false);

        p = 3;
        this->MUL = new Operator<T>(p, "*");
        this->DIV = new Operator<T>(p, "/");
        this->MOD = new Operator<T>(p, "%");

        p = 4;
        this->ADD = new Operator<T>(p, "+");
        this->SUB = new Operator<T>(p, "-");

        p = 5;
        this->SHL = new Operator<T>(p, "<<");
        this->SHR = new Operator<T>(p, ">>");

        p = 7;
        this->EQ = new Operator<T>(p, "==");
        this->NEQ = new Operator<T>(p, "!=");

        p = 8;
        this->AND = new Operator<T>(p, "&");

        p = 9;
        this->XOR = new Operator<T>(p, "^");

        p = 10;
        this->OR = new Operator<T>(p, "|");

        p = 11;
        this->LAND = new Operator<T>(p, "&&");

        p = 12;
        this->LOR = new Operator<T>(p, "||");

        // special operators, not in postfix
        p = -1;
        this->LPAR = new Operator<T>(p, "(");
        this->RPAR = new Operator<T>(p, ")");
        // #undef __operator

        // define apply()
        __assign_func();

        // in priority order
        this->members.clear();
        this->members.push_back(this->LPAR);
        this->members.push_back(this->RPAR);
        this->members.push_back(this->MINUS);
        this->members.push_back(this->PLUS);
        this->members.push_back(this->NOT);
        this->members.push_back(this->LNOT);
        this->members.push_back(this->MUL);
        this->members.push_back(this->DIV);
        this->members.push_back(this->MOD);
        this->members.push_back(this->ADD);
        this->members.push_back(this->SUB);
        this->members.push_back(this->SHL);
        this->members.push_back(this->SHR);
        this->members.push_back(this->EQ);
        this->members.push_back(this->NEQ);
        this->members.push_back(this->AND);
        this->members.push_back(this->XOR);
        this->members.push_back(this->OR);
        this->members.push_back(this->LAND);
        this->members.push_back(this->LOR);
    }

    void _destroy_operators() override
    {
        for (Operator<T>* op : this->members) {
            delete op;
        }
    }

private:
    void __assign_func()
    {
        this->MINUS->apply = [](T x, T y) { return -x; };
        this->PLUS->apply = [](T x, T y) { return x; };

        this->NOT->apply = [](T x, T y) { return ~x; };
        this->LNOT->apply = [](T x, T y) { return !x; };

        this->MUL->apply = [](T x, T y) { return x * y; };
        this->DIV->apply = [](T x, T y) { return x / y; };
        this->MOD->apply = [](T x, T y) { return x % y; };

        this->ADD->apply = [](T x, T y) { return x + y; };
        this->SUB->apply = [](T x, T y) { return x - y; };

        this->SHL->apply = [](T x, T y) { return x << y; };
        this->SHR->apply = [](T x, T y) { return x >> y; };

        this->EQ->apply = [](T x, T y) { return x == y; };
        this->NEQ->apply = [](T x, T y) { return x != y; };

        this->AND->apply = [](T x, T y) { return x & y; };
        this->XOR->apply = [](T x, T y) { return x ^ y; };
        this->OR->apply = [](T x, T y) { return x | y; };

        this->LAND->apply = [](T x, T y) { return x && y; };
        this->LOR->apply = [](T x, T y) { return x || y; };
    }
};


template <typename T>
class CFloatOperatorList : public OperatorList<T>
{
public:
    CFloatOperatorList();
};


#endif