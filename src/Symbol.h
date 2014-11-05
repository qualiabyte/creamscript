
#pragma once

namespace cream {
namespace symbol {

enum CharacterType
{
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

} // end cream::symbol
} // end cream
