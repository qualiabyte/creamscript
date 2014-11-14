
#pragma once

#include <list>
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
    virtual bool isOperation() { return false; }
    Expression* inner = NULL;
    Expression* left = NULL;
    Expression* right = NULL;
};

struct Number : Expression
{
    Number(string value="")
    {
        this->type = "Number";
        this->value = value;
    }
    virtual ~Number() {}
};

struct Identifier : Expression
{
    Identifier(string value="")
    {
        this->type = "Identifier";
        this->value = value;
    }
    virtual ~Identifier() {}
};

struct Operation : Expression
{
    Operation(Token token)
    {
        this->type = "Operation";
        this->token = token;
    }
    virtual ~Operation() {}
    virtual bool isOperation() { return true; }
    Token token;
};

struct Assignment : Operation
{
    Assignment(Token token, Expression* left=0, Expression* right=0)
        : Operation(token)
    {
        this->type = "Assignment";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~Assignment() {}
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
        Statement statement;
        list<Expression*> expressions;

        // Process expression tokens
        for (auto iter = tokens.begin(); iter != tokens.end(); iter++)
        {
            Token token = *iter;
            Expression* expression;

            if (token.type == cream::token::NUMBER)
            {
                expression = new Number(token.value);
            }
            else if (token.type == cream::token::IDENTIFIER)
            {
                expression = new Identifier(token.value);
            }
            else if (token.type == cream::token::ASSIGN)
            {
                expression = new Operation(token);
            }
            else if (token.type == cream::token::WHITESPACE)
            {
                continue;
            }

            expressions.push_back(expression);
        }

        // Process operation expressions
        for (auto iter = expressions.begin(); iter != expressions.end(); iter++)
        {
            Expression* expression = *iter;
            if (expression->type == "Operation")
            {
                Operation* operation = (Operation*) expression;
                auto left = iter; left--;
                auto right = iter; right++;

                if (operation->token.type == cream::token::ASSIGN)
                {
                    auto assignment = new Assignment(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, assignment);
                }
            }
        }

        // Process outer expressions
        if (expressions.size() > 0)
        {
            if (expressions.size() > 1)
                cerr << "Expected one top expression, found "
                     << expressions.size() << endl;
            Expression* topExpression = expressions.front();
            statement.outer = topExpression;
            root.statements.push_back(statement);
        }
        return ast;
    }
};

void testParser()
{
    cout << "Testing Parser" << endl;

    // Construct parser
    Parser parser;
    Lexer lexer;

    {
        // Test empty expression
        auto source = "";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 0);
    }

    {
        // Test value expression
        auto source = "123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 1);
        assert(ast.root->statements[0].outer->type == "Number");
        assert(ast.root->statements[0].outer->value == "123");
    }

    {
        // Test variable expression
        auto source = "abc";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 1);
        assert(ast.root->statements[0].outer->type == "Identifier");
        assert(ast.root->statements[0].outer->value == "abc");
    }

    {
        // Test variable assigment
        auto source = "abc = 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root->statements.size() == 1);
        assert(ast.root->statements[0].outer->type == "Assignment");
        assert(ast.root->statements[0].outer->left->type == "Identifier");
        assert(ast.root->statements[0].outer->left->value == "abc");
        assert(ast.root->statements[0].outer->right->type == "Number");
        assert(ast.root->statements[0].outer->right->value == "123");
    }

    /*
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
