#ifndef _DSA_EXPRESSION_HPP_
#define _DSA_EXPRESSION_HPP_ 1

#include <ostream>
#include <vector>

#include "node.hpp"
#include "tools.h"

// template <typename T>
// class Expression
// {
// public:
//     Expression() {}
//     ~Expression() { clear(); }

//     typename std::vector<std::shared_ptr<const Node<T>>>::const_iterator begin() const
//     {
//         return __expr.cbegin();
//     }
//     typename std::vector<std::shared_ptr<const Node<T>>>::const_iterator end() const
//     {
//         return __expr.cend();
//     }

//     void clear() noexcept { __expr.clear(); }

//     size_t size() const { return __expr.size(); }

//     void push_back(std::shared_ptr<const Node<T>>&& node) { __expr.push_back(node); }
//     void push_back(const std::shared_ptr<const Node<T>>& node) {
//     __expr.push_back(node); }

//     void push_back_operand(const T& value)
//     {
//         auto opd = std::make_shared<const Operand<T>>(value);
//         // LOG("  push value: %s", % *opd);
//         __expr.push_back(opd);
//     }

//     void push_back_operator(std::shared_ptr<const Operator<T>> opt)
//     {
//         // LOG("  push op: %s", % *opt);
//         __expr.push_back(opt);
//     }

// private:
//     std::vector<std::shared_ptr<const Node<T>>> __expr;
// };

template <typename T>
using Expression = std::vector<std::shared_ptr<const Node<T>>>;


template <typename T>
std::ostream& operator<<(std::ostream& os, const Expression<T>& obj)
{
    for (const std::shared_ptr<const Node<T>>& node : obj) {
        os << node->str() << ' ';
    }
    return os;
}

#endif
