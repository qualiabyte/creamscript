
#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "Scanner.h"
#include "Token.h"

namespace cream {
namespace lexer {

using namespace std;

class Lexer
{
public:
    Lexer(string source)
    {
        this->scanner = new Scanner(source);
    }

    // Converts the source string into a series of Token objects.
    vector<Token> tokenize()
    {
        vector<Token> tokens;
        scanner->position = -1;
        while (scanner->remaining() > 0)
        {
            Token token;
            string value;
            char c = scanner->next();

            if (isalpha(c))
            {
                handle_identifier:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (isalnum(next) || next == '_')
                {
                    scanner->seek(1);
                    goto handle_identifier;
                }
                else
                {
                    token = { token::IDENTIFIER, "Identifier", value };
                }
            }
            else if (isdigit(c))
            {
                handle_number:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (isdigit(next) || next == '.')
                {
                    scanner->seek(1);
                    goto handle_number;
                }
                else
                {
                    token = { token::NUMBER, "Number", value };
                }
            }
            else if (isblank(c) || c == '\n')
            {
                handle_whitespace:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (isblank(next) || next == '\n') {
                    scanner->seek(1);
                    goto handle_whitespace;
                }
                else
                {
                    token = { token::WHITESPACE, "Whitespace", value };
                }
            }
            else if (c == '+')
            {
                value += c;
                token = { symbol::PLUS, "Plus", value };
            }
            else if (c == '-')
            {
                value += c;
                token = { symbol::MINUS, "Minus", value };
            }
            else if (c == '*')
            {
                value += c;
                token = { symbol::ASTERISK, "Asterisk", value };
            }
            else if (c == '/')
            {
                value += c;
                token = { symbol::SLASH, "Slash", value };
            }

            if (token.type)
                tokens.push_back(token);
        }
        return tokens;
    }

private:
    Scanner* scanner;
};

void testLexer()
{
    cout << "Testing Lexer" << endl;

    {
        string source = "123 abc";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 3);
        assert(tokens[0].toString() == "Number 123");
        assert(tokens[1].toString() == "Whitespace  ");
        assert(tokens[2].toString() == "Identifier abc");
    }

    {
        string source = "a + b / c * d";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 13);
        assert(tokens[0].toString() == "Identifier a");
        assert(tokens[2].toString() == "Plus +");
        assert(tokens[4].toString() == "Identifier b");
        assert(tokens[6].toString() == "Slash /");
        assert(tokens[8].toString() == "Identifier c");
        assert(tokens[10].toString() == "Asterisk *");
        assert(tokens[12].toString() == "Identifier d");
    }
}

} // end cream::lexer

using Lexer = cream::lexer::Lexer;

} // end cream
