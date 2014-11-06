
#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Symbol.h"

namespace cream {
namespace token {

using namespace std;

struct Token
{
    int type;
    string value;
    string toString()
    {
        return to_string(type) + " " + value;
    }
};

enum TokenType
{
   UNDEFINED,
   WHITESPACE,      // SP*
   LOGICAL_AND,     // && and
   LOGICAL_OR,      // || or
   COMPARE_EQ,      // == is
   COMPARE_LT,      // <
   COMPARE_GT,      // >
   COMPARE_LTE,     // <=
   COMPARE_GTE,     // >=
   UNKNOWN
};

map<string, TokenType> Tokens =
{
    { "&&"  , LOGICAL_AND },
    { "and" , LOGICAL_AND },
    { "||"  , LOGICAL_OR },
    { "or"  , LOGICAL_OR },
    { "=="  , COMPARE_EQ },
    { "is"  , COMPARE_EQ },
    { "<"   , COMPARE_LT },
    { "<="  , COMPARE_LTE },
    { ">"   , COMPARE_GT },
    { ">="  , COMPARE_GTE }
};

vector<Token> tokenize(string s)
{
    vector<Token> tokens;
    for (int i = 0, len = s.length(); i < len; i++)
    {
        Token token;
        char c = s[i];
        string value(1, c);

        if (isalpha(c))
            token = { symbol::ALPHA, value };
        else if (isspace(c))
            token = { token::WHITESPACE, value };
        else if (c == '+')
            token = { symbol::PLUS, value };
        else if (c == '-')
            token = { symbol::MINUS, value };
        else if (c == '*')
            token = { symbol::ASTERISK, value };
        else if (c == '/')
            token = { symbol::SLASH, value };

        if (token.type)
            tokens.push_back(token);
    }
    return tokens;
}

void testTokenizeCharacterSequence()
{
    auto tokens = tokenize("ABC");
    assert(tokens[0].type == symbol::ALPHA);
    assert(tokens[1].type == symbol::ALPHA);
    assert(tokens[2].type == symbol::ALPHA);
    assert(tokens[0].value == "A");
    assert(tokens[1].value == "B");
    assert(tokens[2].value == "C");
}

void testTokenizeArithmeticExpression()
{
    auto tokens = tokenize("a + b");
    assert(tokens[0].type == symbol::ALPHA);
    assert(tokens[1].type == token::WHITESPACE);
    assert(tokens[2].type == symbol::PLUS);
    assert(tokens[3].type == token::WHITESPACE);
    assert(tokens[4].type == symbol::ALPHA);
    assert(tokens[0].value == "a");
    assert(tokens[1].value == " ");
    assert(tokens[2].value == "+");
    assert(tokens[3].value == " ");
    assert(tokens[4].value == "b");

    tokens = tokenize("a * b / c");
    assert(tokens[0].type == symbol::ALPHA);
    assert(tokens[2].type == symbol::ASTERISK);
    assert(tokens[4].type == symbol::ALPHA);
    assert(tokens[6].type == symbol::SLASH);
    assert(tokens[8].type == symbol::ALPHA);
    assert(tokens[0].value == "a");
    assert(tokens[2].value == "*");
    assert(tokens[4].value == "b");
    assert(tokens[6].value == "/");
    assert(tokens[8].value == "c");
}

void testTokenize()
{
    cout << "Testing tokenize" << endl;
    testTokenizeCharacterSequence();
    testTokenizeArithmeticExpression();
}

} // end cream::token

using Token = cream::token::Token;

} // end cream
