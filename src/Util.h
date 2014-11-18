
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Token.h"

namespace cream {
namespace util {

using namespace std;

class Util
{
public:
    template <typename T>
    static list<T> vec2list(vector<T> a)
    {
        list<T> b;
        std::copy(a.begin(), a.end(), std::back_inserter(b));
        return b;
    }

    template <typename T>
    static vector<T> list2vec(list<T> a)
    {
        vector<T> b;
        b.reserve(a.size());
        std::copy(a.begin(), a.end(), std::back_inserter(b));
        return b;
    }
};

} // end cream::util

using Util = cream::util::Util;

} // end cream
