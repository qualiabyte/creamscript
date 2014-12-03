
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Common.h"
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
        addExpressionMetadata(tokenList);
        removeWhitespace(tokenList);
        rewriteKeywords(tokenList);
        rewriteLambdaExpressions(tokenList);
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
    void rewriteKeywords(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto &token = *iter;
            if (token.type == cream::token::IDENTIFIER)
            {
                if (token.value == "return")
                {
                    token.type = cream::token::KEYWORD;
                    token.name = "Return";
                }
            }
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
                    throw CreamError(
                        "Extra closing parenthesis found at "
                        "line " + to_string(token->meta.line) + ", "
                        "column " + to_string(token->meta.column) + "\n"
                    );
                }

                startTokens.pop_back();
                depth--;
            }
        }
    }
    void rewriteLambdaExpressions(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); )
        {
            auto token = *iter;
            if (token.name == "Arrow")
            {
                auto next = iter; next++;
                auto prev = iter; prev--;

                // Replace left side with param list
                if (prev->type != cream::token::EXPRESSION_END)
                {
                    // Insert empty param list
                    auto paramsStart = new Token { cream::token::PARAMS_START, "Params Start", "(" };
                    auto paramsEnd = new Token { cream::token::PARAMS_END, "Params End", ")" };
                    tokenList.insert(iter, *paramsStart);
                    tokenList.insert(iter, *paramsEnd);
                }
                else
                {
                    // Find the param list start
                    auto paramsIter = next;
                    while (paramsIter->meta.position != prev->pair.start && paramsIter != tokenList.begin())
                    {
                        paramsIter--;
                    }
                    auto start = paramsIter;

                    // Convert start token
                    start->type = cream::token::PARAMS_START;
                    start->name = "Params Start";

                    // Convert end token
                    auto end = prev;
                    end->type = cream::token::PARAMS_END;
                    end->name = "Params End";
                }

                // Replace right side with block
                if (next->type != cream::token::BLOCK_START)
                {
                    // Create tokens to insert
                    auto blockIter = next;
                    auto blockStart = new Token { cream::token::BLOCK_START, "Block Start", "{" };
                    auto blockEnd = new Token { cream::token::BLOCK_END, "Block End", "}" };

                    // Set pair metadata
                    cream::token::Pair pair;
                    pair.start = Token::implicitPosition();
                    pair.end = Token::implicitPosition();
                    blockStart->pair = pair;
                    blockEnd->pair = pair;

                    // Set position
                    blockStart->meta.position = pair.start;
                    blockEnd->meta.position = pair.end;

                    // Insert block start after arrow
                    auto start = tokenList.insert(next, *blockStart);

                    // Insert block end before newline
                    while (blockIter->type != cream::token::NEWLINE && blockIter != tokenList.end())
                    {
                        blockIter++;
                    }
                    auto end = tokenList.insert(blockIter, *blockEnd);

                    // Advance main iterator to block start
                    iter = start;
                }
                else
                {
                    // Find end of existing block
                    auto start = next;
                    auto blockIter = next;
                    while (blockIter != tokenList.end() && blockIter->meta.position != start->pair.end)
                    {
                        blockIter++;
                    }
                    auto end = blockIter;

                    // Advance main iterator to block start
                    iter = start;
                }
            }
            else
            {
                iter++;
            }
        }
    }
};

} // end cream::rewriter

using Rewriter = cream::rewriter::Rewriter;

} // end cream
