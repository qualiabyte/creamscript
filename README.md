
# CreamScript

A CoffeeScript-like language that compiles to C++.

## Examples

### Hello World

CreamScript

```coffee
import stdio

using namespace std

int main() ->
  cout << "Hello world!" << endl
  return 1
```

Compiled C++

```cpp
#include <stdio>

using namespace std

int main()
{
  cout << "Hello world!" << endl;
  return 1;
}
```

### Fibonacci

CreamScript

```coffee
int fibonacci(int n) ->
  if n > 1
    return n * fibonacci(n - 1)
  else
    return n

int main() ->
  cout << "Fibonacci(10): " << fibonacci(10) << endl
```

Compiled C++

```cpp
int fibonacci(int n) {
  if (n > 1)
    return n * fibonacci(n - 1);
  else
    return n;
}

int main()
{
  cout << "Fibonacci(10): " << fibonacci(10) << endl;
}
```

## Features

Language

+ ✓ Identifiers
+ ✓ Variables
  + ✗ Definition
  + ✗ Assignment
+ ✗ Keywords
+ ✗ Types
+ ✓ Literals
  + ✓ Numbers
  + ✗ Strings
+ ✓ Blocks
+ ✓ Statements
  + ✗ If Else
  + ✗ For
  + ✗ While
+ ✓ Expressions
  + ✓ Expression Groups
+ ✓ Operations
  + ✓ Arithmetic
    + ✓ Add, Subtract, Multiply, Divide
+ ✗ Functions
  + ✗ Definition
  + ✗ Calls
  + ✗ Lambdas

Compiler

+ ✓ Token
+ ✓ Scanner
+ ✓ Rewriter
+ ✓ Lexer
+ ✓ Parser
+ ✗ Back end
  + ✗ C++ Output
