
#pragma once

#include <string>

namespace cream {
namespace common {

using namespace std;

/**
 * The CreamError class.
 */

class CreamError: public runtime_error
{
public:
    CreamError(const string message="Cream Error")
        : runtime_error(message.c_str())
    {}
};

/**
 * A custom assert method.
 *
 * @param condition The asserted condition.
 * @param message The assertion message.
 */

inline void cassert(bool condition, string message)
{
    if (!condition) throw CreamError(message);
}

} // end cream::common

using CreamError = common::CreamError;
auto cassert = common::cassert;

} // end cream
