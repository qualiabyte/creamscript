
#include "src/Scanner.h"
#include "src/Token.h"

using namespace std;

int main()
{
    cout << "Running tests" << endl;
    cream::token::testTokenize();
    cream::scanner::testScanner();
    cout << "Done!" << endl;
    return 0;
}
