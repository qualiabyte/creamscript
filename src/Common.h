
#pragma once

#include <stdexcept>
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
 * A custom assertion method.
 *
 * @param condition The asserted condition.
 * @param message The assertion message.
 */

inline void creamAssert(bool condition, string message)
{
    if (!condition) throw CreamError(message);
}

} // end cream::common

using CreamError = common::CreamError;
auto creamAssert = common::creamAssert;

} // end cream
