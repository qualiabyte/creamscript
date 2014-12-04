
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

struct Token;

struct Metadata
{
    int line;
    int column;
    int position;
};

struct Pair
{
    int start;
    int end;
    static vector<Token> innerTokens(vector<Token>::iterator start);
    template<typename Iterator> static Iterator endFor(Iterator start);
    template<typename Iterator> static Iterator startFor(Iterator end);
    template<typename Iterator> static void seekToEnd(Iterator & iter, Pair* pair=0);
    template<typename Iterator> static void seekToStart(Iterator & iter, Pair* pair=0);
};

struct Token
{
    int type;
    string name;
    string value;
    Metadata meta;
    Pair pair;
    string toString()
    {
        return name + " " + value;
    }
    string debug()
    {
        return "Token '" + value + "'\n" +
               "  name: " + name + "\n" +
               "  meta: line " + to_string(meta.line) +
                     ", column " + to_string(meta.column) +
                     ", position " + to_string(meta.position) + "\n" +
               "  pair.start: " + to_string(pair.end) + "\n" +
               "  pair.end: " + to_string(pair.end) + "\n";
    }

    // Gets the next position for implicit tokens.
    static int implicitPosition()
    {
        return --lastImplicitPos;
    }
    static int lastImplicitPos;
};

int Token::lastImplicitPos = 0;

/**
 * Gets inner tokens, given a token pair start iterator.
 */

vector<Token>
Pair::innerTokens(vector<Token>::iterator start)
{
    vector<Token> innerTokens;
    auto inner = start; inner++;
    while (inner->meta.position != start->pair.end)
    {
        auto innerToken = *inner;
        innerTokens.push_back(innerToken);
        inner++;
    }
    return innerTokens;
}

/**
 * Gets matching start for iterator at token pair end.
 */

template<typename Iterator>
Iterator Pair::startFor(Iterator end)
{
    auto start = end;
    Pair::seekToStart(start);
    return start;
}

/**
 * Gets matching end for iterator at token pair start.
 */

template<typename Iterator>
Iterator Pair::endFor(Iterator start)
{
    auto end = start;
    Pair::seekToEnd(end);
    return end;
}

/**
 * Seeks iterator to start of a token pair.
 */

template <typename Iterator>
void Pair::seekToStart(Iterator & iter, Pair* pair)
{
    if (!pair) pair = &iter->pair;
    while (iter->meta.position != pair->start)
    {
        iter--;
    }
}

/**
 * Seeks iterator to end of a token pair.
 */

template <typename Iterator>
void Pair::seekToEnd(Iterator & iter, Pair* pair)
{
    if (!pair) pair = &iter->pair;
    while (iter->meta.position != pair->end)
    {
        iter++;
    }
}

enum TokenType
{
    UNDEFINED,
    IDENTIFIER,        // [a-z,A-Z][a-z,A-Z,0-9,_]+
    TYPE,              // int, float, double, char...
    KEYWORD,           // for, while, if, else, return...
    NUMBER,            // [0-9]+(.[0-9]+)?
    STRING,            // "[^"]*"
    WHITESPACE,        // SP*
    NEWLINE,           // \n
    ASSIGN,            // =
    ARROW,             // ->
    PARAMS_START,      // (
    PARAMS_END,        // )
    PARAM_TYPE,        // <type>
    PARAM_NAME,        // <identifier>
    COMMA,             // ,
    VAR_TYPE,          // <type>
    VAR_NAME,          // <identifier>
    OP_ADD,            // +
    OP_INCREMENT,      // ++
    OP_SUBTRACT,       // -
    OP_DECREMENT,      // --
    OP_MULTIPLY,       // *
    OP_DIVIDE,         // /
    BITWISE_AND,       // &
    BITWISE_OR,        // |
    COMPARE_EQ,        // == is
    COMPARE_LT,        // <
    COMPARE_GT,        // >
    COMPARE_LTE,       // <=
    COMPARE_GTE,       // >=
    LOGICAL_AND,       // && and
    LOGICAL_OR,        // || or
    EXPRESSION_START,  // (
    EXPRESSION_END,    // )
    STATEMENT_START,   // \n ...
    STATEMENT_END,     // ... \n
    BLOCK_START,       // INDENT
    BLOCK_END,         // UNINDENT
    INDENT,            //   ...
    UNKNOWN
};

map<string, TokenType> Tokens =
{
    { " "   , WHITESPACE },
    { "\n"  , NEWLINE },
    { "="   , ASSIGN },
    { "+"   , OP_ADD },
    { "++"  , OP_INCREMENT },
    { "-"   , OP_SUBTRACT },
    { "--"  , OP_DECREMENT },
    { "*"   , OP_MULTIPLY },
    { "/"   , OP_DIVIDE },
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

} // end cream::token

using Token = cream::token::Token;
using Pair = cream::token::Pair;

} // end cream
