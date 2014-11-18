
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
    string name;
    string value;
    int line;
    int character;
    string toString()
    {
        return name + " " + value;
    }
};

enum TokenType
{
    UNDEFINED,
    IDENTIFIER,        // [a-z,A-Z][a-z,A-Z,0-9,_]+
    NUMBER,            // [0-9]+(.[0-9]+)?
    WHITESPACE,        // SP*
    NEWLINE,           // \n
    ASSIGN,            // =
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

} // end cream
