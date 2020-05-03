#ifndef _DSA_OPERATORLIST_HPP_
#define _DSA_OPERATORLIST_HPP_ 1

#include <algorithm>
#include <cmath>
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

    // common operators
    Operator<T>* MUL;
    Operator<T>* DIV;
    Operator<T>* ADD;
    Operator<T>* SUB;
    Operator<T>* LPAR;
    Operator<T>* RPAR;
    Operator<T>* EQ;
    Operator<T>* NEQ;
    Operator<T>* MINUS;
    Operator<T>* PLUS;


protected:
    virtual void _init_operators() = 0;

    virtual void _destroy_operators()
    {
        for (Operator<T>* op : this->members) {
            delete op;
        }
    }
};


template <typename T>
class CIntOperatorList : public OperatorList<T>
{
public:
    CIntOperatorList() { this->_init_operators(); }
    ~CIntOperatorList() { this->_destroy_operators(); }

    // integer specific operators
    Operator<T>* MOD;
    Operator<T>* AND;
    Operator<T>* XOR;
    Operator<T>* OR;
    Operator<T>* SHL;
    Operator<T>* SHR;
    Operator<T>* LAND;
    Operator<T>* LOR;
    Operator<T>* NOT;
    Operator<T>* LNOT;


protected:
    void _init_operators() override
    {
        // https://en.cppreference.com/w/c/language/operator_precedence

        int p;
        p = 2;
        this->MINUS = new Operator<T>(p, "-", 1, RIGHT);
        this->PLUS = new Operator<T>(p, "+", 1, RIGHT);
        this->NOT = new Operator<T>(p, "~", 1, RIGHT);
        this->LNOT = new Operator<T>(p, "!", 1, RIGHT);

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


        // define functions
        this->MINUS->apply = [](T x, T _) { return -x; };
        this->PLUS->apply = [](T x, T _) { return x; };

        this->NOT->apply = [](T x, T _) { return ~x; };
        this->LNOT->apply = [](T x, T _) { return !x; };

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
        std::sort(this->members.begin(), this->members.end(),
                  [](const Operator<T>* x, const Operator<T>* y) {
                      return x->repr.size() > y->repr.size();
                  });  // longest first
    }

private:
};


template <typename T>
class CFloatOperatorList : public OperatorList<T>
{
public:
    CFloatOperatorList() { this->_init_operators(); }
    ~CFloatOperatorList() { this->_destroy_operators(); }

    // floating-point number specific operators
    Operator<T>* SIN;
    Operator<T>* COS;
    Operator<T>* EXP;
    Operator<T>* LOG;
    Operator<T>* POW;
    Operator<T>* SQRT;
    Operator<T>* ABS;


protected:
    void _init_operators() override
    {
        // https://en.cppreference.com/w/c/language/operator_precedence

        int p;
        p = 0;  // functions has highest priority
        this->SIN = new Operator<T>(p, "sin", 1, RIGHT);
        this->COS = new Operator<T>(p, "cos", 1, RIGHT);
        this->EXP = new Operator<T>(p, "exp", 1, RIGHT);
        this->LOG = new Operator<T>(p, "log", 1, RIGHT);
        this->POW = new Operator<T>(p, "pow", 2, RIGHT);
        this->SQRT = new Operator<T>(p, "sqrt", 1, RIGHT);
        this->ABS = new Operator<T>(p, "fabs", 1, RIGHT);

        p = 2;
        this->MINUS = new Operator<T>(p, "-", 1, RIGHT);
        this->PLUS = new Operator<T>(p, "+", 1, RIGHT);

        p = 3;
        this->MUL = new Operator<T>(p, "*");

        p = 4;
        this->ADD = new Operator<T>(p, "+");
        this->SUB = new Operator<T>(p, "-");

        // special operators, not in postfix
        p = -1;
        this->LPAR = new Operator<T>(p, "(");
        this->RPAR = new Operator<T>(p, ")");


        // define functions
        this->SIN->apply = [](T x, T _) { return std::sin(x); };
        this->COS->apply = [](T x, T _) { return std::cos(x); };
        this->EXP->apply = [](T x, T _) { return std::exp(x); };
        this->LOG->apply = [](T x, T _) { return std::log(x); };
        this->POW->apply = [](T x, T y) { return std::pow(x, y); };
        this->SQRT->apply = [](T x, T _) { return std::sqrt(x); };
        this->ABS->apply = [](T x, T _) { return std::fabs(x); };

        this->MINUS->apply = [](T x, T _) { return -x; };
        this->PLUS->apply = [](T x, T _) { return x; };

        this->MUL->apply = [](T x, T y) { return x * y; };

        this->ADD->apply = [](T x, T y) { return x + y; };
        this->SUB->apply = [](T x, T y) { return x - y; };


        // in priority order
        this->members.clear();
        this->members.push_back(this->SIN);
        this->members.push_back(this->COS);
        this->members.push_back(this->EXP);
        this->members.push_back(this->LOG);
        this->members.push_back(this->POW);
        this->members.push_back(this->SQRT);
        this->members.push_back(this->ABS);
        this->members.push_back(this->LPAR);
        this->members.push_back(this->RPAR);
        this->members.push_back(this->MINUS);
        this->members.push_back(this->PLUS);
        this->members.push_back(this->MUL);
        this->members.push_back(this->ADD);
        this->members.push_back(this->SUB);
        std::sort(this->members.begin(), this->members.end(),
                  [](const Operator<T>* x, const Operator<T>* y) {
                      return x->repr.size() > y->repr.size();
                  });  // longest first
    }

private:
};

#endif