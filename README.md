
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

using namespace std;

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
    return fibonacci(n - 1) + fibonacci(n - 2)
  else
    return n

int main() ->
  cout << "Fibonacci(10): " << fibonacci(10) << endl
```

Compiled C++

```cpp
int fibonacci(int n) {
  if (n > 1)
    return fibonacci(n - 1) + fibonacci(n - 2);
  else
    return n;
}

int main()
{
  cout << "Fibonacci(10): " << fibonacci(10) << endl;
}
```

### Lambdas

CreamScript

```coffee
sum = (double a, double b) ->
  return a + b

sum(41, 1)
```

Compiled C++

```cpp
auto sum = [] (double a, double b)
{
  return a + b;
}

sum(41, 1);
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
  + ✓ Strings
+ ✓ Blocks
  + ✓ Single line
  + ✗ Multi-line
+ ✓ Statements
  + ✗ If Else
  + ✗ For
  + ✗ While
  + ✓ Return
+ ✓ Expressions
  + ✓ Expression Groups
+ ✓ Operations
  + ✓ Arithmetic
    + ✓ Add, Subtract, Multiply, Divide
+ ✗ Functions
  + ✓ Definition
  + ✓ Lambdas
  + ✓ Parameter Lists
  + ✗ Calls

Compiler

+ ✓ Token
+ ✓ Scanner
+ ✓ Rewriter
+ ✓ Lexer
+ ✓ Parser
+ ✓ Back end
  + ✓ C++ Output
