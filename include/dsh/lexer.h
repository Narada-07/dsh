#ifndef LEXER_H
#define LEXER_H
#include <dsh/command.h>

typedef enum LexerState
{
    INVALID_STATE = -1,
    NORMAL,
    IN_SINGLE_QUOTE,
    IN_DOUBLE_QUOTE,
    BACKSLASH,
    BACKSLASH_IN_DOUBLE_QUOTES,
}LexerState;

typedef enum
{
    EXPECT_NOTHING,
    EXPECT_STDOUT_FILE,
    EXPECT_STDERR_FILE
} ExpectState;

Command tokenize(char input[]);

#endif