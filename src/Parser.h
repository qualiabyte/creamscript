
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
struct Parameter;
struct ParamList;

class CreamError: public runtime_error
{
public:
    CreamError(const string message="Cream Error")
        : runtime_error(message.c_str())
    {}
};

void cassert(bool condition, string message)
{
    if (!condition) throw CreamError(message);
}

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
    Token token;
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

    // Block members
    vector<Statement> statements;

    // Lambda members
    ParamList* paramList = NULL;
    Block* block = NULL;

    // ParamList members
    vector<Parameter> params;

    // Statement members
    Expression* inner = NULL;

    // Binary operation members
    Expression* left = NULL;
    Expression* right = NULL;

    // Unary operation members
    Expression* operand = NULL;
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
};

struct UnaryOperation : Operation
{
    UnaryOperation(Token token, Expression* operand=0)
        : Operation(token)
    {
        this->type = "Unary Operation";
        this->token = token;
        this->operand = operand;
    }
    virtual ~UnaryOperation() {}
};

struct Return : UnaryOperation
{
    Return(Token token, Expression* operand)
        : UnaryOperation(token, operand)
    {
        this->type = "Return";
        this->token = token;
        this->operand = operand;
    }
    virtual ~Return() {}
};

struct BinaryOperation : Operation
{
    BinaryOperation(Token token, Expression* left=0, Expression* right=0)
        : Operation(token)
    {
        this->type = "Binary Operation";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~BinaryOperation() {}
};

struct Addition : BinaryOperation
{
    Addition(Token token, Expression* left=0, Expression* right=0)
        : BinaryOperation(token, left, right)
    {
        this->type = "Addition";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~Addition() {}
};

struct Subtraction : BinaryOperation
{
    Subtraction(Token token, Expression* left=0, Expression* right=0)
        : BinaryOperation(token, left, right)
    {
        this->type = "Subtraction";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~Subtraction() {}
};

struct Multiplication : BinaryOperation
{
    Multiplication(Token token, Expression* left=0, Expression* right=0)
        : BinaryOperation(token, left, right)
    {
        this->type = "Multiplication";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~Multiplication() {}
};

struct Division : BinaryOperation
{
    Division(Token token, Expression* left=0, Expression* right=0)
        : BinaryOperation(token, left, right)
    {
        this->type = "Division";
        this->token = token;
        this->left = left;
        this->right = right;
    }
    virtual ~Division() {}
};

struct Assignment : BinaryOperation
{
    Assignment(Token token, Expression* left=0, Expression* right=0)
        : BinaryOperation(token, left, right)
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
    ExpressionGroup(Token start, Token end, Expression* inner)
        :Expression()
    {
        this->type = "Expression Group";
        this->start = start;
        this->end = end;
        this->inner = inner;
    }
    virtual ~ExpressionGroup() {}
    Token start;
    Token end;
};

struct Statement : Node
{
    Statement() {}
    virtual ~Statement() {}
    bool isEmpty() { return outer == NULL; }
    Expression* outer = NULL;
};

struct Block : Expression
{
    Block(vector<Statement> statements={})
    {
        this->type = "Block";
        this->statements = statements;
    }
    virtual ~Block() {}
};

struct Parameter : Node
{
    Parameter(string paramType, string paramName, string defaultValue="")
        : Node()
    {
        this->type = "Parameter";
        this->token = token;
        this->paramType = paramType;
        this->paramName = paramName;
        this->defaultValue = defaultValue;
    }
    virtual ~Parameter() {}

    string paramType;
    string paramName;
    string defaultValue;
};

struct ParamList : Expression
{
    ParamList(Token start, Token end, vector<Parameter> params)
    {
        this->type = "Parameter List";
        this->start = start;
        this->end = end;
        this->params = params;
    }
    virtual ~ParamList() {}
    Token start;
    Token end;
};

struct Lambda : Expression
{
    Lambda(Token token, ParamList* paramList, Block* block)
        : Expression()
    {
        this->type = "Lambda";
        this->token = token;
        this->paramList = paramList;
        this->block = block;
    }
    virtual ~Lambda()
    {
        delete block;
        delete paramList;
    }
};

struct AST
{
    AST() {}
    virtual ~AST() {}
    Block root;
};

class Parser
{
public:
    Parser()
    {}

    AST parse(vector<Token> tokens)
    {
        AST ast;
        ast.root = parseBlock(tokens);
        return ast;
    }

    Block parseBlock(vector<Token> tokens)
    {
        Block block;
        vector<Statement> blockItems;

        // Process tokens in block
        for (auto iter = tokens.begin(); iter != tokens.end(); iter++)
        {
            // Process statement tokens
            vector<Token> statementTokens;
            while (iter->type != token::NEWLINE && iter != tokens.end())
            {
                Token token = *iter;
                statementTokens.push_back(token);
                iter++;
            }
            Statement statement = parseStatement(statementTokens);
            blockItems.push_back(statement);

            if (iter == tokens.end())
                break;
        }
        block.statements = blockItems;
        return block;
    }

    Statement parseStatement(vector<Token> tokens)
    {
        Statement statement;
        auto expression = parseExpression(tokens);
        statement.outer = expression;
        return statement;
    }

    Expression* parseExpression(vector<Token> tokens)
    {
        auto expressions = parseTokens(tokens);
        processOperations(expressions);

        if (expressions.size() > 1)
            throw CreamError("Expect only one top level expression");

        if (expressions.size() == 0)
            return NULL;

        auto topExpression = expressions.front();
        return topExpression;
    }

    // Creates list of expression objects for given tokens.
    list<Expression*> parseTokens(vector<Token> tokens)
    {
        list<Expression*> expressions;
        for (auto iter = tokens.begin(); iter != tokens.end(); iter++)
        {
            Token token = *iter;
            Expression* expression = NULL;

            if (token.type == cream::token::BLOCK_START)
            {
                auto start = iter;
                auto blockTokens = Pair::innerTokens(start);
                auto block = parseBlock(blockTokens);
                expression = new Block(block);
                Pair::advanceToEnd(iter);
            }
            else if (token.type == cream::token::EXPRESSION_START)
            {
                auto start = iter;
                auto end = Pair::endFor(start);
                auto innerTokens = Pair::innerTokens(start);
                auto innerExpression = parseExpression(innerTokens);
                expression = new ExpressionGroup(*start, *end, innerExpression);
                Pair::advanceToEnd(iter);
            }
            else if (token.type == cream::token::PARAMS_START)
            {
                // TODO: Parse param tokens
                auto start = iter;
                auto end = Pair::endFor(start);
                auto innerTokens = Pair::innerTokens(start);
                vector<Parameter> params {};
                expression = new ParamList(*start, *end, params);
                Pair::advanceToEnd(iter);
            }
            else if (token.type == cream::token::ARROW)
            {
                expression = new Operation(token);
            }
            else if (token.type == cream::token::NUMBER)
            {
                expression = new Number(token.value);
            }
            else if (token.type == cream::token::IDENTIFIER)
            {
                expression = new Identifier(token.value);
            }
            else if (token.type == cream::token::KEYWORD)
            {
                if (token.name == "Return")
                {
                    // TODO: Implicit return expressions
                    auto next = iter; next++;
                    vector<Token> operandTokens { *next };
                    auto operand = parseExpression(operandTokens);
                    expression = new Return(token, operand);
                    iter = next;
                }
            }
            else if (token.type == cream::token::ASSIGN ||
                     token.type == cream::token::OP_ADD ||
                     token.type == cream::token::OP_SUBTRACT ||
                     token.type == cream::token::OP_MULTIPLY ||
                     token.type == cream::token::OP_DIVIDE)
            {
                expression = new Operation(token);
            }
            else if (token.type == cream::token::WHITESPACE)
            {
                continue;
            }
            else
            {
                cerr << "Skipping unknown token during parse: '"
                     << token.value << "'" << endl;
                continue;
            }

            if (expression != NULL)
                expressions.push_back(expression);
        }
        return expressions;
    }

    // Converts operations to their specific types.
    void processOperations(list<Expression*> &expressions)
    {
        for (auto iter = expressions.begin(); iter != expressions.end(); iter++)
        {
            Expression* expression = *iter;
            if (expression->type == "Operation")
            {
                Operation* operation = (Operation*) expression;
                auto left = iter; left--;
                auto right = iter; right++;

                if (operation->token.type == cream::token::ARROW)
                {
                    auto paramList = (ParamList*) *left;
                    auto block = (Block*) *right;
                    auto lambda = new Lambda(operation->token, paramList, block);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, lambda);
                }
                if (operation->token.type == cream::token::ASSIGN)
                {
                    auto assignment = new Assignment(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, assignment);
                }
                else if (operation->token.type == cream::token::OP_ADD)
                {
                    auto addition = new Addition(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, addition);
                }
                else if (operation->token.type == cream::token::OP_SUBTRACT)
                {
                    auto subtraction = new Subtraction(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, subtraction);
                }
                else if (operation->token.type == cream::token::OP_MULTIPLY)
                {
                    auto multiplication = new Multiplication(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, multiplication);
                }
                else if (operation->token.type == cream::token::OP_DIVIDE)
                {
                    auto division = new Division(operation->token, *left, *right);
                    expressions.erase(left);
                    expressions.erase(right);
                    iter = expressions.erase(iter);
                    expressions.insert(iter, division);
                }
            }
        }
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
        assert(ast.root.statements.size() == 0);
    }

    {
        // Test value expression
        auto source = "123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Number");
        assert(ast.root.statements[0].outer->value == "123");
    }

    {
        // Test variable expression
        auto source = "abc";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Identifier");
        assert(ast.root.statements[0].outer->value == "abc");
    }

    {
        // Test variable assigment
        auto source = "abc = 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Assignment");
        assert(ast.root.statements[0].outer->left->type == "Identifier");
        assert(ast.root.statements[0].outer->left->value == "abc");
        assert(ast.root.statements[0].outer->right->type == "Number");
        assert(ast.root.statements[0].outer->right->value == "123");
    }

    {
        // Test multiple statements
        auto source = "a = 1\n"
                      "b = 2\n"
                      "c = 3";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 3);
        assert(ast.root.statements[0].outer->token.value == "=");
        assert(ast.root.statements[0].outer->left->value == "a");
        assert(ast.root.statements[0].outer->right->value == "1");
        assert(ast.root.statements[1].outer->token.value == "=");
        assert(ast.root.statements[1].outer->left->value == "b");
        assert(ast.root.statements[1].outer->right->value == "2");
        assert(ast.root.statements[2].outer->token.value == "=");
        assert(ast.root.statements[2].outer->left->value == "c");
        assert(ast.root.statements[2].outer->right->value == "3");
    }

    {
        // Test expression group
        auto source = "(a + b)";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Expression Group");
        assert(ast.root.statements[0].outer->inner->type == "Addition");
        assert(ast.root.statements[0].outer->inner->left->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->right->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->left->value == "a");
        assert(ast.root.statements[0].outer->inner->right->value == "b");
    }

    {
        // Test expression tree
        auto source = "((a + b) / (c * d))";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Expression Group");
        assert(ast.root.statements[0].outer->inner->type == "Division");
        assert(ast.root.statements[0].outer->inner->left->type == "Expression Group");
        assert(ast.root.statements[0].outer->inner->left->inner->type == "Addition");
        assert(ast.root.statements[0].outer->inner->left->inner->left->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->left->inner->right->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->left->inner->left->value == "a");
        assert(ast.root.statements[0].outer->inner->left->inner->right->value == "b");
        assert(ast.root.statements[0].outer->inner->right->inner->type == "Multiplication");
        assert(ast.root.statements[0].outer->inner->right->inner->right->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->right->inner->right->type == "Identifier");
        assert(ast.root.statements[0].outer->inner->right->inner->left->value == "c");
        assert(ast.root.statements[0].outer->inner->right->inner->right->value == "d");
    }

    {
        // Test return statement
        auto source = "return 42";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Return");
        assert(ast.root.statements[0].outer->operand->type == "Number");
        assert(ast.root.statements[0].outer->operand->value == "42");
    }

    {
        // Test block
        auto source = "() -> return 42";
        auto tokens = lexer.tokenize(source);
        assert(tokens.size() == 7);
        vector<Token> blockTokens({ tokens[3], tokens[4], tokens[5], tokens[6] });
        auto ast = parser.parse(blockTokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Block");
        assert(ast.root.statements[0].outer->statements[0].outer->type == "Return");
        assert(ast.root.statements[0].outer->statements[0].outer->operand->type == "Number");
        assert(ast.root.statements[0].outer->statements[0].outer->operand->value == "42");
    }

    {
        // Test lambda expression
        auto source = "() -> return 42";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Lambda");
        assert(ast.root.statements[0].outer->paramList->type == "Parameter List");
        assert(ast.root.statements[0].outer->paramList->params.size() == 0);
        assert(ast.root.statements[0].outer->block->type == "Block");
        assert(ast.root.statements[0].outer->block->statements.size() == 1);
        assert(ast.root.statements[0].outer->block->statements[0].outer->type == "Return");
        assert(ast.root.statements[0].outer->block->statements[0].outer->operand->type == "Number");
        assert(ast.root.statements[0].outer->block->statements[0].outer->operand->value == "42");
    }

    /*
    {
        // Test assignment operators
        auto source = "abc += 123";
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

using Parser = parser::Parser;
using AST = parser::AST;
using Block = parser::Block;
using Statement = parser::Statement;
using Expression = parser::Expression;

} // end cream
