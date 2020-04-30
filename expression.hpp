// TODO:
// use shared_ptr and weak_ptr
// encapsulatation of pri, repr, val...
#ifndef _DSA_EXPRESSION_H_
#define _DSA_EXPRESSION_H_ 1

#include <string>
#include <vector>

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
                      bool left_associative = true)
        : pri(priority),
          repr(representation),
          operands(num_operands),
          l_assoc(left_associative)
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

    int pri;           // priority, LOWER is prior
    std::string repr;  // representation when printed out
    int operands;      // unary: 1, binary: 2
    bool l_assoc;      // true: left-associative, false: right-associative
    std::function<T(T, T)> apply;  // for unary, second parameter is ignored
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
