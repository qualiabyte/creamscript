
#pragma once

#include <string>
#include <cassert>

namespace cream {
namespace scanner {

using namespace std;

class Scanner
{
public:
    Scanner(string source="")
        : source(source),
          position(0)
    {}

    // Seeks `n` characters from current position.
    char seek(int n)
    {
        position += n;
        return at(position);
    }

    // Peeks at `offset` from current position.
    char peek(int offset = 0)
    {
        return at(position + offset);
    }

    // Gets next character, seeking forward.
    char next()
    {
        return seek(1);
    }

    // Gets previous character, seeking backward.
    char prev()
    {
        return seek(-1);
    }

    // Gets current character, without moving.
    char current()
    {
        return peek(0);
    }

    // Gets character from the source at `pos`.
    char at(int pos)
    {
        if (pos >= 0 && pos < source.length())
            return source.at(pos);
        else
            return 0;
    }

    // Moves to position `pos` in the source.
    void to(int pos)
    {
        this->position = pos;
    }

    // Gets the number of remaining characters.
    int remaining()
    {
        return length() - (position + 1);
    }

    // Gets the length of the source in bytes.
    int length()
    {
        return (int) source.length();
    }

    // Loads a new source to scan.
    void load(string source)
    {
        this->source = source;
        rewind();
    }

    // Rewind the position.
    void rewind()
    {
        position = 0;
    }

    string source;
    int position;
};

void testScanner()
{
    cout << "Testing Scanner" << endl;
    string source = "123 abc";
    Scanner scanner(source);
    assert(scanner.length() == 7);
    assert(scanner.peek() == '1');
    assert(scanner.next() == '2');
    assert(scanner.peek(1) == '3');
    assert(scanner.peek(-1) == '1');
    assert(scanner.at(3) == ' ');

    scanner.to(4);
    assert(scanner.remaining() == 2);
    assert(scanner.peek() == 'a');
    assert(scanner.next() == 'b');
    assert(scanner.current() == 'b');
    assert(scanner.prev() == 'a');
    assert(scanner.seek(2) == 'c');
    assert(scanner.seek(-1) == 'b');
}

} // end cream::scanner

using Scanner = cream::scanner::Scanner;

} // end cream
