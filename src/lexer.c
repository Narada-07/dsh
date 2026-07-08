#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dsh/lexer.h>

void tokenize(char input[1024], char* argv[100])
{
    int argc = 0;
    int len = 0;
    LexerState state = NORMAL;
    LexerState prev_state = INVALID_STATE;
    char curr_arg[1024];
    curr_arg[0] = '\0';
    char* stdout_redirect;
    char* stderr_redirect;
    for (int i = 0; input[i] != '\0'; ++i)
    {
        switch (state)
        {
        case (NORMAL):
            if (input[i] == '\'') state = IN_SINGLE_QUOTE;
            else if (input[i] == '"') state = IN_DOUBLE_QUOTE;
            else if (input[i] == '\\')
            {
                state = BACKSLASH;
                prev_state = NORMAL;
            }
            else if (isspace(input[i]))
            {
                if (len > 0)
                {
                    argv[argc++] = strdup(curr_arg);
                    len = 0;
                    curr_arg[0] = '\0';
                }
            }
            else
            {
                curr_arg[len++] = input[i];
                curr_arg[len] = '\0';
            }
            break;
        case (IN_SINGLE_QUOTE):
            if (input[i] == '\'') state = NORMAL;
            else
            {
                curr_arg[len++] = input[i];
                curr_arg[len] = '\0';
            }
            break;
        case (IN_DOUBLE_QUOTE):
            if (input[i] == '"') state = NORMAL;
            else if (input[i] == '\\') state = BACKSLASH_IN_DOUBLE_QUOTES;
            else
            {
                curr_arg[len++] = input[i];
                curr_arg[len] = '\0';
            }
            break;
        case (BACKSLASH):
            curr_arg[len++] = input[i];
            curr_arg[len] = '\0';
            state = NORMAL;
            prev_state = INVALID_STATE;
            break;
        case (BACKSLASH_IN_DOUBLE_QUOTES):
            if (input[i] == '\\' || input[i] == '"')
            {
                curr_arg[len++] = input[i];
                curr_arg[len] = '\0';
                state = IN_DOUBLE_QUOTE;
            }
            else
            {
                curr_arg[len++] = input[i - 1];
                curr_arg[len++] = input[i];
                curr_arg[len] = '\0';
                state = IN_DOUBLE_QUOTE;
            }
            break;
        }
    }
    if (len > 0)
    {
        argv[argc++] = strdup(curr_arg);
        len = 0;
        curr_arg[0] = '\0';
    }
    argv[argc] = NULL;

    return;

}
