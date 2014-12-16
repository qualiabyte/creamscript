
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
    }

    // Destroys the Lexer.
    ~Lexer()
    {
        delete scanner;
        delete rewriter;
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

        // Token metadata
        token::Metadata meta;
        meta.line = 1;
        meta.column = 1;
        meta.position = 1;

        // Line pointers
        auto currentLine = make_shared<Line>();
        auto sourceLines = make_shared<vector<Line*>>();
        sourceLines->push_back(&*currentLine);

        // Scanner setup
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
                    if (meta.column == 1)
                    {
                        auto indent = value;
                        if (Line::firstIndentSize == 0)
                            Line::firstIndentSize = indent.size();
                        currentLine->indent = indent;
                    }
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
                else if (next == '>')
                {
                    value += next;
                    scanner->seek(1);
                    token = { token::ARROW, "Arrow", value };
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
            else if (c == ',')
            {
                value += c;
                token = { token::COMMA, "Comma", value };
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

            // Set line
            token.line = currentLine;

            // Set lines
            token.lines = sourceLines;

            // Set token metadata
            token.meta = meta;

            // Update position
            meta.position += token.value.size();

            // Update line and column
            if (token.type == token::NEWLINE)
            {
                meta.column = 1;
                meta.line += token.value.size();
                currentLine = make_shared<Line>();
                sourceLines->push_back(&*currentLine);
            }
            else
            {
                meta.column += token.value.size();
                currentLine->content += token.value;
            }

            // Add the token
            if (token.type)
                tokens.push_back(token);
        }
        return tokens;
    }

private:
    Scanner* scanner;
    Rewriter* rewriter;
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
        // Test Token Pairs
        string source = "((a + b) / (c * d))";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 13);
        assert(tokens[0].pair.start == 1);
        assert(tokens[0].pair.end == 19);
        assert(tokens[1].pair.start == 2);
        assert(tokens[1].pair.end == 8);
        assert(tokens[11].pair.start == 12);
        assert(tokens[11].pair.end == 18);
        assert(tokens[12].pair.start == 1);
        assert(tokens[12].pair.end == 19);
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
        assert(tokens[0].meta.line == 1);
        assert(tokens[0].meta.column == 1);
        assert(tokens[0].meta.position == 1);
        assert(tokens[2].meta.line == 1);
        assert(tokens[2].meta.column == 5);
        assert(tokens[2].meta.position == 5);
        assert(tokens[4].meta.line == 2);
        assert(tokens[4].meta.column == 1);
        assert(tokens[4].meta.position == 7);
        assert(tokens[6].meta.line == 2);
        assert(tokens[6].meta.column == 5);
        assert(tokens[6].meta.position == 11);
        assert(tokens[8].meta.line == 4);
        assert(tokens[8].meta.column == 1);
        assert(tokens[8].meta.position == 14);
        assert(tokens[10].meta.line == 4);
        assert(tokens[10].meta.column == 5);
        assert(tokens[10].meta.position == 18);
    }

    {
        // Test Lambda Expression
        string source = "() -> return 123";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 9);
        assert(tokens[0].toString() == "Params Start (");
        assert(tokens[1].toString() == "Params End )");
        assert(tokens[2].toString() == "Arrow ->");
        assert(tokens[3].toString() == "Block Start {");
        assert(tokens[4].toString() == "Return return");
        assert(tokens[5].toString() == "Expression Start (");
        assert(tokens[6].toString() == "Number 123");
        assert(tokens[7].toString() == "Expression End )");
        assert(tokens[8].toString() == "Block End }");
    }

    {
        // Test Implicit Param List
        string source = "-> return 123";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 9);
        assert(tokens[0].toString() == "Params Start (");
        assert(tokens[1].toString() == "Params End )");
        assert(tokens[2].toString() == "Arrow ->");
    }

    {
        // Test Param List
        string source = "(double a, double b) -> return 123";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 14);
        assert(tokens[0].toString() == "Params Start (");
        assert(tokens[1].toString() == "Identifier double");
        assert(tokens[2].toString() == "Identifier a");
        assert(tokens[3].toString() == "Comma ,");
        assert(tokens[4].toString() == "Identifier double");
        assert(tokens[5].toString() == "Identifier b");
        assert(tokens[6].toString() == "Params End )");
    }

    {
        // Test Return Keyword
        string source = "return 123";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens.size() == 4);
        assert(tokens[0].type == cream::token::KEYWORD);
        assert(tokens[0].toString() == "Return return");
    }

    {
        // Test Lines
        string source = "() ->\n"
                        "  a = 1\n"
                        "  b = 2";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        assert(tokens[0].lines->size() == 3);
        assert(tokens[0].lines == tokens[1].lines);
        assert(tokens[0].lines->at(0)->content == "() ->");
        assert(tokens[0].lines->at(1)->content == "  a = 1");
        assert(tokens[0].lines->at(2)->content == "  b = 2");
        assert(tokens[0].lines->at(0)->indent == "");
        assert(tokens[0].lines->at(1)->indent == "  ");
        assert(tokens[0].lines->at(2)->indent == "  ");
        assert(tokens[0].lines->at(0)->indentLevel() == 0);
        assert(tokens[0].lines->at(1)->indentLevel() == 1);
        assert(tokens[0].lines->at(2)->indentLevel() == 1);
    }
}

} // end cream::lexer

using Lexer = cream::lexer::Lexer;

} // end cream
