// TODO:
// use shared_ptr and weak_ptr
// encapsulatation of pri, repr, val...
#ifndef _DSA_EXPRESSION_HPP_
#define _DSA_EXPRESSION_HPP_ 1

#include <string>
#include <vector>

enum Associativity { LEFT, RIGHT, NONE };

template <typename T>
class Node
{
public:
    Node() {}
    virtual ~Node() {}
    virtual std::string str() const = 0;
};

template <typename T>
class Operator : public Node<T>
{
public:
    Operator() {}
    explicit Operator(int priority,
                      const std::string& representation,
                      int num_operands = 2,
                      Associativity associativity = LEFT)
        : pri(priority),
          repr(representation),
          operands(num_operands),
          assoc(associativity)
    {
    }

    std::string str() const override
    {
#ifndef NDEBUG
        if (operands == 1)
            return "u" + repr;
#endif
        return repr;
    }

    int pri;           // priority, LOWER is prior, >= 0
    std::string repr;  // representation when printed out or parsed
    int operands;      // unary: 1, binary: 2
    Associativity assoc;

    // for unary, second parameter is ignored
    // TODO: support more than 2 parameters
    std::function<T(T, T)> apply;
};


template <typename T>
class Operand : public Node<T>
{
public:
    Operand() {}
    explicit Operand(T value) : val(value) {}

    std::string str() const override { return std::to_string(val); }

    T val;  // value
};


template <typename T>
using Expression = std::vector<Node<T>*>;



template <typename T>
std::ostream& operator<<(std::ostream& os, const Node<T>& obj)
{
    os << obj.str();
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Expression<T>& obj)
{
    for (Node<T>* node : obj) {
        os << node->str() << ' ';
    }
    return os;
}

#endif
