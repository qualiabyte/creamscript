
#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "Rewriter.h"
#include "Scanner.h"
#include "Token.h"

namespace cream {
namespace lexer {

using namespace std;

class Lexer
{
public:
    // Constructs a new Lexer instance.
    Lexer(string source="")
    {
        scanner = new Scanner();
        scanner->load(source);
        rewriter = new Rewriter();
        resetMetadata();
    }

    // Destroys the Lexer.
    ~Lexer()
    {
        delete scanner;
        delete rewriter;
    }

    // Resets the Lexer metadata.
    void resetMetadata()
    {
        line = 1;
        character = 1;
    }

    // Converts the current source string to Tokens.
    vector<Token> tokenize()
    {
        return tokenize(scanner->source);
    }

    // Converts the source string into a series of Token objects.
    vector<Token> tokenize(string source)
    {
        auto tokens = scan(source);
        tokens = rewrite(tokens);
        return tokens;
    }

    // Rewrites tokens to simplify parsing.
    vector<Token> rewrite(vector<Token> tokens)
    {
        return rewriter->rewrite(tokens);
    }

    // Scans source for Token objects.
    vector<Token> scan(string source)
    {
        vector<Token> tokens;
        scanner->source = source;
        scanner->position = -1;
        while (scanner->remaining() > 0)
        {
            Token token;
            string value;
            char c = scanner->next();

            // Tokenize Characters
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
            else if (isblank(c))
            {
                handle_whitespace:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (isblank(next))
                {
                    scanner->seek(1);
                    goto handle_whitespace;
                }
                else
                {
                    token = { token::WHITESPACE, "Whitespace", value };
                }
            }
            else if (c == '\n')
            {
                handle_newline:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (next == '\n')
                {
                    scanner->seek(1);
                    goto handle_newline;
                }
                else
                {
                    token = { token::NEWLINE, "Newline", value };
                }
            }
            else if (c == '=')
            {
                handle_equal:

                value += scanner->peek();
                char next = scanner->peek(1);
                if (next == '=')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::COMPARE_EQ, "Compare Eq", value };
                }
                else
                {
                    token = { token::ASSIGN, "Assign", value };
                }
            }
            else if (c == '+')
            {
                handle_plus:

                value += c;
                char next = scanner->peek(1);
                if (next == '+')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::OP_INCREMENT, "Increment", value };
                }
                else
                {
                    token = { token::OP_ADD, "Add", value };
                }
            }
            else if (c == '-')
            {
                handle_minus:

                value += c;
                char next = scanner->peek(1);
                if (next == '-')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::OP_DECREMENT, "Decrement", value };
                }
                else
                {
                    token = { token::OP_SUBTRACT, "Subtract", value };
                }
            }
            else if (c == '*')
            {
                value += c;
                token = { token::OP_MULTIPLY, "Multiply", value };
            }
            else if (c == '/')
            {
                value += c;
                token = { token::OP_DIVIDE, "Divide", value };
            }
            else if (c == '&')
            {
                handle_ampersand:

                value += c;
                char next = scanner->peek(1);
                if (next == '&')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::LOGICAL_AND, "And", value };
                }
                else
                {
                    token = { token::BITWISE_AND, "Bitwise And", value };
                }
            }
            else if (c == '|')
            {
                handle_pipe:

                value += c;
                char next = scanner->peek(1);
                if (next == '|')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::LOGICAL_OR, "Or", value };
                }
                else
                {
                    token = { token::BITWISE_OR, "Bitwise Or", value };
                }
            }
            else if (c == '(')
            {
                value += c;
                token = { token::EXPRESSION_START, "Expression Start", value };
            }
            else if (c == ')')
            {
                value += c;
                token = { token::EXPRESSION_END, "Expression End", value };
            }

            // Rewrite Identifiers
            if (token.type == token::IDENTIFIER)
            {
                if (token.value == "and")
                {
                    token = { token::LOGICAL_AND, "And", token.value };
                }
                else if (token.value == "or")
                {
                    token = { token::LOGICAL_OR, "Or", token.value };
                }
            }

            // Set token metadata
            token.line = line;
            token.character = character;

            // Update line and character
            if (token.type == token::NEWLINE)
            {
                character = 1;
                line += token.value.size();
            }
            else
            {
                character += token.value.size();
            }

            // Add the token
            if (token.type)
                tokens.push_back(token);
        }
        resetMetadata();
        return tokens;
    }

private:
    Scanner* scanner;
    Rewriter* rewriter;
    int line;
    int character;
};

void testLexer()
{
    cout << "Testing Lexer" << endl;

    {
        // Test single use
        Lexer lexer("123");
        auto tokens1 = lexer.tokenize();
        assert(tokens1.size() == 1);
        assert(tokens1[0].toString() == "Number 123");
    }

    {
        // Test multiple invocations
        Lexer lexer;
        assert(lexer.tokenize("123")[0].toString() == "Number 123");
        assert(lexer.tokenize("abc")[0].toString() == "Identifier abc");
    }

    {
        // Test whitespace rewriting
        Lexer lexer;
        auto source = "a = 1";
        auto tokens = lexer.tokenize(source);
        assert(tokens.size() == 3);
    }

    {
        // Test Numbers and Identifiers
        string source = "123 abc";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 2);
        assert(tokens[0].toString() == "Number 123");
        assert(tokens[1].toString() == "Identifier abc");
    }

    {
        // Test Add, Divide, Multiply, Subtract
        string source = "a + b / c * d - e";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 9);
        assert(tokens[0].toString() == "Identifier a");
        assert(tokens[1].toString() == "Add +");
        assert(tokens[2].toString() == "Identifier b");
        assert(tokens[3].toString() == "Divide /");
        assert(tokens[4].toString() == "Identifier c");
        assert(tokens[5].toString() == "Multiply *");
        assert(tokens[6].toString() == "Identifier d");
        assert(tokens[7].toString() == "Subtract -");
        assert(tokens[8].toString() == "Identifier e");
    }

    {
        // Test Increment and Decrement
        string source = "a++ + --b";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 5);
        assert(tokens[0].toString() == "Identifier a");
        assert(tokens[1].toString() == "Increment ++");
        assert(tokens[2].toString() == "Add +");
        assert(tokens[3].toString() == "Decrement --");
        assert(tokens[4].toString() == "Identifier b");
    }

    {
        // Test Logical Operators
        string source = "a and b or c && d || e";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 9);
        assert(tokens[0].toString() == "Identifier a");
        assert(tokens[1].toString() == "And and");
        assert(tokens[2].toString() == "Identifier b");
        assert(tokens[3].toString() == "Or or");
        assert(tokens[4].toString() == "Identifier c");
        assert(tokens[5].toString() == "And &&");
        assert(tokens[6].toString() == "Identifier d");
        assert(tokens[7].toString() == "Or ||");
        assert(tokens[8].toString() == "Identifier e");
    }

    {
        // Test Expression Groups
        string source = "((a + b) / (c * d))";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 13);
        assert(tokens[0].toString() == "Expression Start (");
        assert(tokens[1].toString() == "Expression Start (");
        assert(tokens[5].toString() == "Expression End )");
        assert(tokens[7].toString() == "Expression Start (");
        assert(tokens[11].toString() == "Expression End )");
        assert(tokens[12].toString() == "Expression End )");
    }

    {
        // Test Statement Groups
        string source =
                "a = 1\n"
                "b = 2\n"
                "c = 3";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 11);
        assert(tokens[3].toString() == "Newline \n");
        assert(tokens[7].toString() == "Newline \n");
    }

    {
        // Test token metadata
        Lexer lexer;
        auto source =
                "a = 1\n"
                "b = 2\n\n"
                "c = 3";
        auto tokens = lexer.tokenize(source);
        assert(tokens[0].line == 1);
        assert(tokens[0].character == 1);
        assert(tokens[2].line == 1);
        assert(tokens[2].character == 5);
        assert(tokens[4].line == 2);
        assert(tokens[4].character == 1);
        assert(tokens[6].line == 2);
        assert(tokens[6].character == 5);
        assert(tokens[8].line == 4);
        assert(tokens[8].character == 1);
        assert(tokens[10].line == 4);
        assert(tokens[10].character == 5);
    }
}

} // end cream::lexer

using Lexer = cream::lexer::Lexer;

} // end cream
