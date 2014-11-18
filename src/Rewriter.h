
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Token.h"
#include "Util.h"

namespace cream {
namespace rewriter {

using namespace std;

class Rewriter {
public:
    Rewriter() {}
    virtual ~Rewriter() {}
    vector<Token> rewrite(vector<Token> tokens)
    {
        auto tokenList = Util::vec2list(tokens);
        for (auto iter = tokenList.begin(); iter != tokenList.end(); )
        {
            auto token = *iter;
            if (token.name == "Whitespace")
                iter = tokenList.erase(iter);
            else
                iter++;
        }
        return Util::list2vec(tokenList);
    }
};

} // end cream::rewriter

using Rewriter = cream::rewriter::Rewriter;

} // end cream
