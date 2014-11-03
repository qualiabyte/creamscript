
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

enum TokenType {
    SPACE,          // SP
    ALPHA,          // A-Z a-z
    DIGIT,          // 0-9
    PAREN_LEFT,     // (
    PAREN_RIGHT,    // )
    HYPHEN,         // -
    UNDERSCORE,     // _
    SIGN_EQ,        // =
    SIGN_LT,        // <
    SIGN_GT,        // >
    QUOTE_DOUBLE,   // "
    QUOTE_SINGLE    // '
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
    }
    return tokens;
}

void testTokenize()
{
  cout << "Testing tokenize" << endl;
  auto tokens = tokenize("ABC");
  assert(tokens[0].type == ALPHA);
  assert(tokens[1].type == ALPHA);
  assert(tokens[2].type == ALPHA);
  assert(strcmp(tokens[0].value, "A") == 0);
  assert(strcmp(tokens[1].value, "B") == 0);
  assert(strcmp(tokens[2].value, "C") == 0);
}

int main()
{
    cout << "Running tests" << endl;
    testTokenize();
    cout << "Done!" << endl;
    return 0;
}
