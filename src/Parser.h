
#pragma once

#include <string>
#include <vector>
#include "Lexer.h"
#include "Token.h"

namespace cream {
namespace parser {

using namespace std;

struct AST;
struct Node;
struct Block;
struct Statement;
struct Expression;

struct Node
{
    Node()
    {
        this->type = "Node";
    }
    virtual ~Node() {}
    virtual vector<Node> children() {}
    virtual string toString()
    {
        return type + " " + value;
    }
    string type;
    string value;
};

struct Expression : Node
{
    Expression(string value="")
    {
        this->type = "Expression";
        this->value = value;
    }
    virtual ~Expression() {}
    Expression* inner = NULL;
};

struct NumberExpression : Expression
{
    NumberExpression(string value="")
    {
        this->type = "NumberExpression";
        this->value = value;
    }
    virtual ~NumberExpression() {}
};

struct Assignment : Expression
{
    Node* left;
    Node* right;
};

struct ExpressionGroup : Expression
{
    Expression* inner;
};

struct Statement : Node
{
    Expression* outer;
};

struct Block : Node
{
    vector<Statement> statements;
};

struct AST
{
    AST()
    {
        root = new Block;
    }

    ~AST()
    {
        delete root;
    }

    Block* root;
};

class Parser
{
public:
    Parser()
    {}

    AST parse(vector<Token> tokens)
    {
        AST ast;
        Block& root = *ast.root;

        for (auto token : tokens)
        {
            Statement statement;
            Expression* expression;

            if (token.type == cream::token::NUMBER)
            {
                expression = new NumberExpression(token.value);
            }

            statement.outer = expression;
            root.statements.push_back(statement);
        }
        return ast;
    }
};

void testParser()
{
    {
        // Test parser construction
        Parser parser;
        Lexer lexer("");
        auto tokens = lexer.tokenize();
        AST ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 0);
    }

    {
        // Test value expression
        Parser parser;
        Lexer lexer;
        auto source = "123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 1);
        assert(ast.root->statements[0].outer->type == "NumberExpression");
        assert(ast.root->statements[0].outer->value == "123");
    }

    /*
    {
        // Test variable expression
        auto source = "abc";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
    }

    {
        // Test variable assigment
        auto source = "abc = 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
    }

    {
        // Test type declaration
        auto source = "int abc";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
    }

    {
        // Test variable initialisation
        auto source = "int abc = 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
    }

    {
        // Test comparision expression
        auto source = "abc <= 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
    }

    {
        // Test logical operation
        auto source = "true && 123";
    }

    {
        // Test expression groups
        auto source = "(true && (abc or def))";
    }

    {
        // Test multiple statements
        auto source = "a = 1\n"
                      "b = 2\n"
                      "c = 3";
    }

    {
        // Test explicit block
        auto source = "{\n"
                      "  a = 1\n"
                      "  b = 2\n"
                      "  c = 3"
                      "}";
    }

    {
        // Test implicit block
        auto source = "if a = 1\n"
                      "  b = 2\n"
                      "else\n"
                      "  b = 3"
    }
    */
}

} // end cream::parser
} // end cream
