
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
        string output = compileBlock(ast.root);
        return output;
    }
    string compileBlock(Block block)
    {
        string output;
        int len = block.statements.size();
        for (int i = 0; i < len; i++)
        {
            Statement statement = block.statements.at(i);
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
        auto output = compiler.compile(source);
        auto expected = "a = 1;";
        assert(output == expected);
    }

    {
        // Test multi-statement compilation
        auto source = "a = 1\n"
                        "b = 2";
        auto output = compiler.compile(source);
        auto expected = "a = 1;\n"
                          "b = 2;";
        assert(output == expected);
    }

    /*

    {
        // Test lambda expression
        auto source = "() -> return 42";
        auto expected = "[] () { return 42; };";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda param
        auto source = "(double a) -> return a * a";
        auto expected = "[] (double a, double b) { return a * b; }";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda params
        auto source = "(double a, double b) -> return a * b";
        auto expected = "[] (double a, double b) { return a * b; }";
        auto output = compiler.compile(source);
        assert(output == expected);
    }

    {
        // Test lambda assignment
        auto source = "multiply = (double a, double b) -> return a * b";
        auto expected = "auto multiply = [] (double a, double b) { return a * b; }";
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
