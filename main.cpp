
#include "src/Lexer.h"
#include "src/Scanner.h"
#include "src/Token.h"

using namespace std;

int main()
{
    cout << "Running tests" << endl;
    cream::lexer::testLexer();
    cream::scanner::testScanner();
    cout << "Done!" << endl;
    return 0;
}
