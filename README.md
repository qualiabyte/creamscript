
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

