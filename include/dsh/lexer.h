#ifndef LEXER_H
#define LEXER_H

typedef enum
{
    INVALID_STATE = -1,
    NORMAL,
    IN_SINGLE_QUOTE,
    IN_DOUBLE_QUOTE,
    BACKSLASH,
    BACKSLASH_IN_DOUBLE_QUOTES,
    REDIRECT_OUT,
    REDIRECT_ERR,
    APPEND_OUT,
    APPEND_ERR
}LexerState;

void tokenize(char input[], char* argv[]);

#endif