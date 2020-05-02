/* convinient functions */
#ifndef _DSA_TOOLS_H_
#define _DSA_TOOLS_H_ 1

#include <boost/format.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>

/* debug only */
#ifndef NDEBUG
// guard syscall error
#define G(expr)                         \
    if ((expr) < 0) {                   \
        perror("\e[31m" #expr "\e[0m"); \
        exit(-1);                       \
    }

// guard an error by value
#define GG(expr, err_value)             \
    if ((expr) == err_value) {          \
        perror("\e[31m" #expr "\e[0m"); \
        exit(-1);                       \
    }

#define LOG(fmt, ...) std::cerr << boost::format(fmt "\n") __VA_ARGS__
#define LOGN(fmt, ...) std::cerr << boost::format(fmt) __VA_ARGS__

#else
#define G(expr) (expr)
#define GG(expr, err_value) (expr)
#define LOG(...)
#define LOGN(...)
#endif
/* end debug only */


#define FOR(type, i, start, end) for (type i = (start); i < (end); i++)

#endif