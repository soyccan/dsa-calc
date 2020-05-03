/* convinient functions */
#ifndef _DSA_TOOLS_H_
#define _DSA_TOOLS_H_ 1

#include <boost/format.hpp>
#include <iostream>

/* debug only */
#ifndef NDEBUG
#define LOG(fmt, ...) std::cerr << boost::format(fmt "\n") __VA_ARGS__
#define LOGN(fmt, ...) std::cerr << boost::format(fmt) __VA_ARGS__

#else
#define LOG(...)
#define LOGN(...)
#endif
/* end debug only */


#define FOR(type, i, start, end) for (type i = (start); i < (end); i++)

#endif