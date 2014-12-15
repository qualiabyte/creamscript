
#pragma once

#include <string>
#include <vector>
#include "Lexer.h"
#include "Parser.h"

namespace cream {
namespace compiler {

using namespace std;

class Backend
{
public:
    Backend() {}
    virtual ~Backend() {}
    virtual string compile(AST ast) {}
};

class CppBackend : Backend
{
public:
    CppBackend() {}
    virtual ~CppBackend() {}

    string compile(AST ast)
    {
        string output = compileStatements(ast.root.statements);
        return output;
    }

    string compileBlock(Block* block)
    {
        string output;
        string padding = (block->statements.size() > 1) ? "\n" : " ";
        output += "{" + padding;
        output += compileStatements(block->statements) + padding;
        output += "}";
        return output;
    }

    string compileStatements(vector<Statement> statements)
    {
        string output;
        int len = statements.size();
        for (int i = 0; i < len; i++)
        {
            Statement statement = statements.at(i);
            string result = compileStatement(statement);
            output += result;
            if (i < len - 1)
                output += "\n";
        }
        return output;
    }

    string compileStatement(Statement statement)
    {
        string output = compileExpression(statement.outer) + ";";
        return output;
    }

    string compileExpression(Expression* expression)
    {
        string output;
        if (expression->type == "Assignment")
        {
            output += compileExpression(expression->left);
            output += " = ";
            output += compileExpression(expression->right);
        }
        else if (expression->type == "Lambda")
        {
            output += compileLambda((Lambda*) expression);
        }
        else if (expression->type == "Return")
        {
            output += compileReturn((Return*) expression);
        }
        else if (expression->left && expression->right)
        {
            auto binOp = dynamic_cast<BinaryOperation*>(expression);
            output += compileBinaryOperation(binOp);
        }
        else if (expression->type == "Identifier")
        {
            output += expression->value;
        }
        else if (expression->type == "Number")
        {
            output += expression->value;
        }
        return output;
    }

    string compileLambda(Lambda* lambda)
    {
        string output;
        output += compileLambdaCaptures(lambda) + " ";
        output += compileLambdaParams(lambda) + " ";
        output += compileLambdaBlock(lambda);
        return output;
    }

    string compileLambdaCaptures(Lambda* lambda)
    {
        return "[]";
    }

    string compileLambdaParams(Lambda* lambda)
    {
        string output = "(";
        auto params = lambda->paramList->params;
        for (auto iter = params.begin(); iter != params.end(); iter++)
        {
            auto param = *iter;
            auto next = iter; next++;
            output += param.paramType + " ";
            output += param.paramName;
            if (next != params.end())
                output += ", ";
        }
        output += ")";
        return output;
    }

    string compileLambdaBlock(Lambda* lambda)
    {
        return compileBlock(lambda->block);
    }

    string compileReturn(Return* returnExpr)
    {
        return "return " + compileExpression(returnExpr->operand);
    }

    string compileBinaryOperation(BinaryOperation* binOp)
    {
        string output;
        output += compileExpression(binOp->left) + " ";
        output += compileOperator(&binOp->token) + " ";
        output += compileExpression(binOp->right);
        return output;
    }

    string compileOperator(Token* opToken)
    {
        string output;
        output += opToken->value;
        return output;
    }
 };

class Compiler
{
public:
    Compiler()
    {
        this->lexer = new Lexer;
        this->parser = new Parser;
        this->backend = (Backend*) new CppBackend;
    }

    virtual ~Compiler()
    {
        delete lexer;
        delete parser;
        delete backend;
    }

    string compile(string source)
    {
        auto tokens = lexer->tokenize(source);
        auto ast = parser->parse(tokens);
        auto output = backend->compile(ast);
        return output;
    }

    Lexer* lexer;
    Parser* parser;
    Backend* backend;
};

void testCompiler()
{
    cout << "Testing Compiler" << endl;

    // Construct compiler
    Compiler compiler;

    {
        // Test basic compilation
        auto source = "a = 1";
        auto expected = "a = 1;";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test multi-statement compilation
        auto source = "a = 1\n"
                      "b = 2";
        auto expected = "a = 1;\n"
                        "b = 2;";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda expression
        auto source = "() -> return 42";
        auto expected = "[] () { return 42; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test binary operator compilation
        auto source = "a * a";
        auto expected = "a * a;";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda param
        auto source = "(double a) -> return a * a";
        auto expected = "[] (double a) { return a * a; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda params
        auto source = "(double a, double b) -> return a * b";
        auto expected = "[] (double a, double b) { return a * b; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    /*
    {
        // Test lambda assignment
        auto source = "multiply = (double a, double b) -> return a * b";
        auto expected = "auto multiply = [] (double a, double b) { return a * b; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test hello world
        auto source = "int main() ->\n"
                        "  cout << \"Hello World!\"";
        auto expected = "int main() {\n"
                        "  cout << \"Hello World!\";\n"
                        "}";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test variable declaration
        auto source   = "int a";
        auto expected = "int a;"
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test variable declaration with definition
        auto source   = "int a = 1";
        auto expected = "int a = 1;"
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test variable assignment
        auto source   = "b = 1";
        auto expected = "b = 1;"
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test c++ syntax
        auto source   = "cout << \"Hello World\"";
        auto expected = "cout << \"Hello World\"";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test preprocessor directives
        auto source   = "#include <string>";
        auto expected = "#include <string>";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test header inclusion
        auto source   = "import <string>";
        auto expected = "#include <string>";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test source file inclusion
        auto source   = "import "Vector3f";
        auto expected = "#include "Vector3f";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test function definition
        auto source   = "int multiply(int a, int b) -> return a * b";
        auto expected = "int multiply(int a, int b) { return a * b; }";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test function calls
        auto source   = "multiply(a, b)";
        auto expected = "multiply(a, b);";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda functions
        auto source   = "auto multiply = (double a, double b) -> return a * b";
        auto expected = "auto multiply = [] (int a, int b) { return a * b; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test explicit return type
        auto source   = "auto multiply = int (int a, int b) -> return a * b";
        auto expected = "auto multiply = [] (int a, int b) -> int { return a * b; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }
    */
}

} // end cream::compiler

using Compiler = compiler::Compiler;

} // end cream
