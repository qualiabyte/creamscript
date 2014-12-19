
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
        removeEmptyLines(tokenList);
        addIndents(tokenList);
        removeWhitespace(tokenList);
        rewriteIndents(tokenList);
        rewriteKeywords(tokenList);
        rewriteReturnExpressions(tokenList);
        rewriteLambdaExpressions(tokenList);
        return Util::list2vec(tokenList);
    }
    void addIndents(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto const& token = *iter;
            auto prev = iter; prev--;

            if (token.meta.line == 1)
                continue;

            if (token.meta.column == 1)
            {
                int thisLevel = token.line->indentLevel();
                int prevLevel = prev->line->indentLevel();
                int shift = thisLevel - prevLevel;
                if (shift > 0)
                {
                    if (shift > 1)
                    {
                        throw CreamError(
                            "Unexpected indent on line " +
                             to_string(token.meta.line) + "\n"
                        );
                    }
                    else
                    {
                        // Add one indent
                        Token indentToken { cream::token::INDENT, "Indent", "  " };
                        tokenList.insert(iter, indentToken);
                    }
                }
                else if (shift < 0)
                {
                    for (int i = 0; i > shift; i--)
                    {
                        // Add any outdents
                        Token outdentToken { cream::token::OUTDENT, "Outdent", "" };
                        tokenList.insert(iter, outdentToken);
                    }
                }
            }
        }
    }
    void removeEmptyLines(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto const& token = *iter;
            if (token.name == "Newline" && token.line->isEmpty())
            {
                iter = tokenList.erase(iter);
                iter--;
            }
        }
    }
    void removeWhitespace(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto const& token = *iter;
            if (token.name == "Whitespace")
            {
                iter = tokenList.erase(iter);
                iter--;
            }
        }
    }
    void rewriteKeywords(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto & token = *iter;
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
    void rewriteIndents(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto & token = *iter;
            if (token.type == cream::token::INDENT)
            {
                // Replace indent with block start
                Token blockStart { cream::token::BLOCK_START, "Block start", "{" };
                token = blockStart;
            }
            else if (token.type == cream::token::OUTDENT)
            {
                // Replace outdent with block end
                Token blockEnd { cream::token::BLOCK_END, "Block end", "}" };
                token = blockEnd;
            }
        }
    }
    void rewriteReturnExpressions(list<Token> &tokenList)
    {
        auto listBegin = tokenList.begin();
        auto listEnd = tokenList.end();
        for (auto iter = listBegin; iter != listEnd; iter++)
        {
            auto const& token = *iter;
            auto next = iter; next++;
            if (token.type == cream::token::KEYWORD)
            {
                if (token.value != "return")
                    continue;
                if (next->type == cream::token::EXPRESSION_START)
                    continue;

                // Create implicit pair
                auto expressionStart = new Token { cream::token::EXPRESSION_START, "Expression Start", "(" };
                auto expressionEnd = new Token { cream::token::EXPRESSION_END, "Expression End", ")" };
                Token::makeImplicitPair(*expressionStart, *expressionEnd);

                // Find statement end (TODO: Rewrite newlines)
                auto newline = Token::nextNewline(iter, listEnd);

                // Insert implicit start before next
                tokenList.insert(next, *expressionStart);

                // Insert implicit end before statement end
                tokenList.insert(newline, *expressionEnd);
            }
        }
    }
    void addExpressionMetadata(list<Token> &tokenList)
    {
        list<Token*> startTokens;
        int depth = 0;
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto & token = *iter;
            Token* start;
            Token* end;
            if (token.name == "Expression Start")
            {
                start = &token;
                startTokens.push_back(start);
                depth++;
            }
            else if (token.name == "Expression End")
            {
                end = &token;
                start = startTokens.back();

                token::Pair pair = { start->meta.position, end->meta.position };
                start->pair = pair;
                end->pair = pair;

                if (depth == 0)
                {
                    throw CreamError(
                        "Extra closing parenthesis found at "
                        "line " + to_string(token.meta.line) + ", "
                        "column " + to_string(token.meta.column) + "\n"
                    );
                }

                startTokens.pop_back();
                depth--;
            }
        }
    }
    void rewriteLambdaExpressions(list<Token> &tokenList)
    {
        for (auto iter = tokenList.begin(); iter != tokenList.end(); iter++)
        {
            auto const& token = *iter;
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
                    auto end = prev;
                    auto start = Pair::startFor(end);

                    // Convert start token
                    start->type = cream::token::PARAMS_START;
                    start->name = "Params Start";

                    // Convert end token
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
                    // Advance iterator to block start
                    auto start = next;
                    iter = start;
                }
            }
        }
    }
};

} // end cream::rewriter

using Rewriter = cream::rewriter::Rewriter;

} // end cream
