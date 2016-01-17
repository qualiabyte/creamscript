
#pragma once

#include <list>
#include <string>
#include <vector>
#include "Common.h"
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
struct Function;
struct Lambda;

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

    // Variable Declaration members
    Expression* variable = NULL;

    // Function Definition members
    Expression* function = NULL;

    // Function members
    string returnType;
    string functionName;
    Lambda* lambda;

    // Variable members
    string varType;
    string varName;

    // Binary operation members
    Expression* left = NULL;
    Expression* right = NULL;

    // Unary operation members
    Expression* operand = NULL;
};

struct Variable : Expression
{
    Variable(Token typeToken, Token nameToken)
    {
        this->type = "Variable";
        this->typeToken = typeToken;
        this->nameToken = nameToken;
        this->varType = typeToken.value;
        this->varName = nameToken.value;
    }
    virtual ~Variable() {}
    Token typeToken;
    Token nameToken;
};

struct VariableDeclaration : Expression
{
    VariableDeclaration(Variable* variable)
    {
        this->type = "Variable Declaration";
        this->variable = variable;
        this->value = this->variable->varType + " " + this->variable->varName;
    }
    virtual ~VariableDeclaration() {}
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

struct Function : Expression
{
    Function(Token typeToken, Token nameToken, Lambda* lambda) : Expression()
    {
        this->type = "Function";
        this->typeToken = typeToken;
        this->nameToken = nameToken;
        this->lambda = lambda;
        this->returnType = typeToken.value;
        this->functionName = nameToken.value;
        this->block = lambda->block;
    }
    virtual ~Function()
    {
        delete lambda;
    }
    Token typeToken;
    Token nameToken;
};

struct FunctionDefinition : Expression
{
    FunctionDefinition(Function* function) : Expression()
    {
        this->type = "Function Definition";
        this->function = (Expression*) function;
    }
    virtual ~FunctionDefinition()
    {
        delete function;
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
        block.statements = parseStatements(tokens);
        return block;
    }

    vector<Statement> parseStatements(vector<Token> tokens)
    {
        vector<Statement> statements;
        for (auto iter = tokens.begin(); iter != tokens.end(); iter++)
        {
            // Process statement tokens
            vector<Token> statementTokens;
            while (iter->type != cream::token::NEWLINE && iter != tokens.end())
            {
                Token token = *iter;
                if (token.type == cream::token::BLOCK_START)
                {
                    // Add block start
                    auto blockStart = iter;
                    statementTokens.push_back(*blockStart);
                    iter++;

                    // Add inner tokens
                    while (iter->meta.position != blockStart->pair.end)
                    {
                        auto inner = iter;
                        statementTokens.push_back(*inner);
                        iter++;
                    }

                    // Add block end
                    auto blockEnd = iter;
                    statementTokens.push_back(*blockEnd);
                    iter++;
                }
                else
                {
                    // Add token
                    statementTokens.push_back(token);
                    iter++;
                }
            }
            Statement statement = parseStatement(statementTokens);
            statements.push_back(statement);

            // Break after last token
            if (iter == tokens.end())
                break;
        }
        return statements;
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
        processFunctions(expressions);

        if (expressions.size() > 1)
            throw CreamError("Expect only one top level expression");

        if (expressions.size() == 0)
            return NULL;

        auto topExpression = expressions.front();
        return topExpression;
    }

    // Parses parameter list given the inner tokens.
    vector<Parameter> parseParams(vector<Token> paramTokens)
    {
        vector<Parameter> params;
        for (auto iter = paramTokens.begin(); iter != paramTokens.end(); iter++)
        {
            auto type = iter;
            auto name = iter + 1;
            auto comma = iter + 2;

            Parameter param(type->value, name->value);
            params.push_back(param);

            if (comma == paramTokens.end())
                break;
            else
                iter = comma;
        }
        return params;
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
                Pair::seekToEnd(iter);
            }
            else if (token.type == cream::token::EXPRESSION_START)
            {
                auto start = iter;
                auto end = Pair::endFor(start);
                auto innerTokens = Pair::innerTokens(start);
                auto innerExpression = parseExpression(innerTokens);
                expression = new ExpressionGroup(*start, *end, innerExpression);
                Pair::seekToEnd(iter);
            }
            else if (token.type == cream::token::PARAMS_START)
            {
                auto start = iter;
                auto end = Pair::endFor(start);
                auto innerTokens = Pair::innerTokens(start);
                auto params = parseParams(innerTokens);
                expression = new ParamList(*start, *end, params);
                Pair::seekToEnd(iter);
            }
            else if (token.type == cream::token::ARROW)
            {
                expression = new Operation(token);
            }
            else if (token.type == cream::token::NUMBER)
            {
                expression = new Number(token.value);
            }
            else if (token.type == cream::token::TYPE)
            {
                auto next = iter; next++;

                // Variable Declaration
                auto typeToken = *iter;
                auto nameToken = *next;
                auto variable = new Variable(typeToken, nameToken);
                expression = new VariableDeclaration(variable);
                iter = next;
            }
            else if (token.type == cream::token::IDENTIFIER)
            {
                expression = new Identifier(token.value);
            }
            else if (token.type == cream::token::KEYWORD)
            {
                if (token.name == "Return")
                {
                    auto next = iter; next++;
                    auto start = next;
                    auto operandTokens = Pair::innerTokens(start);
                    auto operand = parseExpression(operandTokens);
                    expression = new Return(token, operand);
                    Pair::seekToEnd(iter, &start->pair);
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
                else if (operation->token.type == cream::token::ASSIGN)
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

    // Creates functions for sequences of variable declaration + lambda.
    void processFunctions(list<Expression*> &expressions)
    {
        for (auto iter = expressions.begin(); iter != expressions.end(); iter++)
        {
            auto next = iter; next++;
            Expression* expression = *iter;
            Expression* second = *next;
            if (expression->type == "Variable Declaration" &&
                second->type == "Lambda")
            {
                auto declaration = (VariableDeclaration*) expression;
                auto variable = (Variable*) declaration->variable;

                // Build function
                Token type = variable->typeToken;
                Token name = variable->nameToken;
                Lambda* lambda = (Lambda*) second;
                Function* function = new Function(type, name, lambda);

                // Build definition
                auto functionDefinition = new FunctionDefinition(function);

                // Replace with function definition
                expressions.erase(next);
                iter = expressions.erase(iter);
                expressions.insert(iter, functionDefinition);
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
        assert(tokens.size() == 9);
        vector<Token> blockTokens({ tokens[3], tokens[4], tokens[5], tokens[6], tokens[7], tokens[8] });
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

    {
        // Test parameter list
        auto source = "(double a, double b) -> return a + b";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Lambda");
        assert(ast.root.statements[0].outer->paramList->type == "Parameter List");
        assert(ast.root.statements[0].outer->paramList->params.size() == 2);
        assert(ast.root.statements[0].outer->paramList->params[0].paramType == "double");
        assert(ast.root.statements[0].outer->paramList->params[0].paramName == "a");
        assert(ast.root.statements[0].outer->paramList->params[1].paramType == "double");
        assert(ast.root.statements[0].outer->paramList->params[1].paramName == "b");
    }

    {
        // Test multi-statement block
        auto source = "() ->\n"
                      "  a = 1\n"
                      "  b = 2\n";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Lambda");
        assert(ast.root.statements[0].outer->paramList->type == "Parameter List");
        assert(ast.root.statements[0].outer->paramList->params.size() == 0);
        assert(ast.root.statements[0].outer->block->type == "Block");
        assert(ast.root.statements[0].outer->block->statements.size() == 2);
        assert(ast.root.statements[0].outer->block->statements[0].outer->type == "Assignment");
        assert(ast.root.statements[0].outer->block->statements[1].outer->type == "Assignment");
    }

    {
        // Test variable declaration
        auto source = "int abc";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements[0].outer->type == "Variable Declaration");
        assert(ast.root.statements[0].outer->value == "int abc");
        assert(ast.root.statements[0].outer->variable->varType == "int");
        assert(ast.root.statements[0].outer->variable->varName == "abc");
    }

    {
        // Test variable definition
        auto source = "int abc = 123";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Assignment");
        assert(ast.root.statements[0].outer->left->type == "Variable Declaration");
        assert(ast.root.statements[0].outer->left->value == "int abc");
        assert(ast.root.statements[0].outer->left->variable->varType == "int");
        assert(ast.root.statements[0].outer->left->variable->varName == "abc");
        assert(ast.root.statements[0].outer->right->type == "Number");
        assert(ast.root.statements[0].outer->right->value == "123");
    }

    {
        // Test function definition
        auto source = "int main() -> return 1";
        auto tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);
        assert(ast.root.statements.size() == 1);
        assert(ast.root.statements[0].outer->type == "Function Definition");
        assert(ast.root.statements[0].outer->function->type == "Function");
        assert(ast.root.statements[0].outer->function->functionName == "main");
        assert(ast.root.statements[0].outer->function->block->statements.size() == 1);
        assert(ast.root.statements[0].outer->function->block->statements[0].outer->type == "Return");
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
using Function = parser::Function;
using FunctionDefinition = parser::FunctionDefinition;
using Block = parser::Block;
using Statement = parser::Statement;
using Expression = parser::Expression;
using BinaryOperation = parser::BinaryOperation;
using UnaryOperation = parser::UnaryOperation;
using Addition = parser::Addition;
using Subtraction = parser::Subtraction;
using Division = parser::Division;
using Multiplication = parser::Multiplication;
using Lambda = parser::Lambda;
using Parameter = parser::Parameter;
using ParamList = parser::ParamList;
using Return = parser::Return;

} // end cream
