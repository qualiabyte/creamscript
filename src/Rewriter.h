
#pragma once

#include <list>
#include <stdexcept>
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
        removeWhitespace(tokenList);
        addExpressionMetadata(tokenList);
        return Util::list2vec(tokenList);
    }
    void removeWhitespace(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); )
        {
            auto token = *iter;
            if (token.name == "Whitespace")
                iter = tokenList.erase(iter);
            else
                iter++;
        }
    }
    void addExpressionMetadata(list<Token> &tokenList)
    {
        list<Token*> startTokens;
        int depth = 0;
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto token = &(*iter);
            Token* start;
            Token* end;
            if (token->name == "Expression Start")
            {
                start = token;
                startTokens.push_back(start);
                depth++;
            }
            else if (token->name == "Expression End")
            {
                end = token;
                start = startTokens.back();

                token::Pair pair = { start->meta.position, end->meta.position };
                start->pair = pair;
                end->pair = pair;

                if (depth == 0)
                {
                    throw invalid_argument
                    (
                        "Extra closing parenthesis found at line " + to_string(token->meta.line) +
                        ", column " + to_string(token->meta.column) + "\n"
                    );
                }

                startTokens.pop_back();
                depth--;
            }
        }
    }
};

} // end cream::rewriter

using Rewriter = cream::rewriter::Rewriter;

} // end cream
