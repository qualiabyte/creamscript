
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
        string source = "a = 1";
        string output = compiler.compile(source);
        string expected = "a = 1;";
        assert(output == expected);
    }

    {
        // Test multi-statement compilation
        string source = "a = 1\n"
                        "b = 2";
        string output = compiler.compile(source);
        string expected = "a = 1;\n"
                          "b = 2;";
        assert(output == expected);
    }
}

} // end cream::compiler

using Compiler = compiler::Compiler;

} // end cream
