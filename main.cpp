
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

namespace cream {
namespace token {

enum TokenType {
    SPACE,          // SP
    ALPHA,          // A-Z a-z
    DIGIT,          // 0-9
    EXCLAMATION,    // !
    QUESTION,       // ?
    PIPE,           // |
    AMPERSAND,      // &
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    COMMA,          // ,
    PERIOD,         // .
    SLASH,          // /
    BACKSLASH,      // \\
    COLON,          // :
    SEMICOLON,      // ;
    PAREN_LEFT,     // (
    PAREN_RIGHT,    // )
    BRACKET_LEFT,   // [
    BRACKET_RIGHT,  // ]
    BRACE_LEFT,     // {
    BRACE_RIGHT,    // }
    UNDERSCORE,     // _
    SIGN_EQ,        // =
    SIGN_LT,        // <
    SIGN_GT,        // >
    SIGN_NUMBER,    // #
    SIGN_DOLLAR,    // $
    SIGN_PERCENT,   // %
    SIGN_AT,        // @
    QUOTE_DOUBLE,   // "
    QUOTE_SINGLE,   // '
    QUOTE_BACKTICK, // `
    CARET,          // ^
    TILDE           // ~
};

struct Token {
    int type;
    char* value;
};

Token* tokenize(string s)
{
    int len = s.length();
    Token* tokens = new Token[len];
    int tokenCount = 0;
    char c;
    for (int i = 0; i < len; i++)
    {
        c = s[i];
        char* value = new char[4];
        sprintf(value, "%c", c);
        if (isalpha(c))
            tokens[tokenCount++] = { ALPHA, value };
        if (isspace(c))
            tokens[tokenCount++] = { SPACE, value };
        if (c == '+')
            tokens[tokenCount++] = { PLUS, value };
        if (c == '-')
            tokens[tokenCount++] = { MINUS, value };
        if (c == '*')
            tokens[tokenCount++] = { ASTERISK, value };
        if (c == '/')
            tokens[tokenCount++] = { SLASH, value };
    }
    return tokens;
}

void testTokenizeCharacterSequence()
{
    auto tokens = tokenize("ABC");
    assert(tokens[0].type == ALPHA);
    assert(tokens[1].type == ALPHA);
    assert(tokens[2].type == ALPHA);
    assert(strcmp(tokens[0].value, "A") == 0);
    assert(strcmp(tokens[1].value, "B") == 0);
    assert(strcmp(tokens[2].value, "C") == 0);
}

void testTokenizeArithmeticExpression()
{
    auto tokens = tokenize("a + b");
    assert(tokens[0].type == ALPHA);
    assert(tokens[1].type == SPACE);
    assert(tokens[2].type == PLUS);
    assert(tokens[3].type == SPACE);
    assert(tokens[4].type == ALPHA);
    assert(strcmp(tokens[0].value, "a") == 0);
    assert(strcmp(tokens[1].value, " ") == 0);
    assert(strcmp(tokens[2].value, "+") == 0);
    assert(strcmp(tokens[3].value, " ") == 0);
    assert(strcmp(tokens[4].value, "b") == 0);

    tokens = tokenize("a * b / c");
    assert(tokens[0].type == ALPHA);
    assert(tokens[2].type == ASTERISK);
    assert(tokens[4].type == ALPHA);
    assert(tokens[6].type == SLASH);
    assert(tokens[8].type == ALPHA);
    assert(strcmp(tokens[0].value, "a") == 0);
    assert(strcmp(tokens[2].value, "*") == 0);
    assert(strcmp(tokens[4].value, "b") == 0);
    assert(strcmp(tokens[6].value, "/") == 0);
    assert(strcmp(tokens[8].value, "c") == 0);
}

void testTokenize()
{
    cout << "Testing tokenize" << endl;
    testTokenizeCharacterSequence();
    testTokenizeArithmeticExpression();
}

} // end cream::token
} // end cream

int main()
{
    cout << "Running tests" << endl;
    cream::token::testTokenize();
    cout << "Done!" << endl;
    return 0;
}
