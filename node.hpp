#ifndef _DSA_NODE_HPP_
#define _DSA_NODE_HPP_ 1

#include <functional>
#include <ostream>
#include <sstream>
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
    enum Associativity { LEFT, RIGHT, NONE };

    Operator() {}
    Operator(int priority,
             std::string&& representation,
             int num_operands = 2,
             Associativity associativity = LEFT)
        : __pri(priority),
          __repr(representation),
          __operands(num_operands),
          __assoc(associativity)
    {
    }

    // setter
    const std::function<T(T, T)>& apply(std::function<T(T, T)>&& apply)
    {
        return __apply = apply;
    }

    // getter
    int pri() const { return __pri; }
    const std::string& repr() const { return __repr; }
    const std::function<T(T, T)>& apply() const { return __apply; }
    int operands() const { return __operands; }
    Associativity assoc() const { return __assoc; }

    std::string str() const override
    {
#ifndef NDEBUG
        if (__operands == 1)
            return "u" + __repr;
#endif
        return __repr;
    }

private:
    int __pri;           // priority, LOWER is prior, >= 0
    std::string __repr;  // representation when printed out or parsed
    int __operands;      // unary: 1, binary: 2
    Associativity __assoc;

    // for unary, second parameter is ignored
    // TODO: support more than 2 parameters
    std::function<T(T, T)> __apply;
};


template <typename T>
class Operand : public Node<T>
{
public:
    Operand() {}
    Operand(T value) : val(value) {}

    std::string str() const override
    {
        std::ostringstream os;
        os << val;
        return os.str();
    }

    T val;  // value
};


template <typename T>
std::ostream& operator<<(std::ostream& os, const Node<T>& obj)
{
    os << obj.str();
    return os;
}

#endif