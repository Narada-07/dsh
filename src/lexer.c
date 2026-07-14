#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dsh/lexer.h>
#include <dsh/command.h>

//Command struct stores the argv stdout_file to write to nad 
Command tokenize(char input[1024])
{
    Command curr_cmd = {.argv = {NULL}, .stdout_file = NULL, .stderr_file = NULL, .stdout_mode = INVALID_MODE,
                                 .stderr_mode = INVALID_MODE};
    int argc = 0;
    int len = 0;
    LexerState state = NORMAL;
    char curr_arg[1024];
    curr_arg[0] = '\0';
    ExpectState expect = EXPECT_NOTHING;
    //Check current state and execute according behaviour
    for (int i = 0; input[i] != '\0'; ++i)
    {
        switch (state)
        {
        case (NORMAL):
            if (input[i] == '\'') state = IN_SINGLE_QUOTE;
            else if (input[i] == '"') state = IN_DOUBLE_QUOTE;
            else if (input[i] == '\\') state = BACKSLASH;
            else if (isspace(input[i]))
            {
                if (len > 0)
                {
                    if (expect == EXPECT_STDOUT_FILE)
                    {
                        if (curr_cmd.stdout_file != NULL) free(curr_cmd.stdout_file);
                        curr_cmd.stdout_file = strdup(curr_arg);
                        expect = EXPECT_NOTHING;
                    }
                    else if (expect == EXPECT_STDERR_FILE)
                    {
                        if (curr_cmd.stderr_file != NULL) free(curr_cmd.stderr_file);
                        curr_cmd.stderr_file = strdup(curr_arg);
                        expect = EXPECT_NOTHING;
                    }
                    else if (strcmp(curr_arg, ">") == 0 || strcmp(curr_arg, "1>") == 0)
                    {
                        expect = EXPECT_STDOUT_FILE;
                        curr_cmd.stdout_mode = REDIRECT_TRUNCATE;
                    }
                    else if (strcmp(curr_arg, ">>") == 0 || strcmp(curr_arg, "1>>") == 0)
                    {
                        expect = EXPECT_STDOUT_FILE;
                        curr_cmd.stdout_mode = REDIRECT_APPEND;
                    }
                    else if (strcmp(curr_arg, "2>") == 0)
                    {
                        expect = EXPECT_STDERR_FILE;
                        curr_cmd.stderr_mode = REDIRECT_TRUNCATE;
                    }
                    else if (strcmp(curr_arg, "2>>") == 0)
                    {
                        expect = EXPECT_STDERR_FILE;
                        curr_cmd.stderr_mode = REDIRECT_APPEND;
                    }
                    else curr_cmd.argv[argc++] = strdup(curr_arg);
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
        if (expect == EXPECT_STDOUT_FILE)
        {
            if (curr_cmd.stdout_file != NULL) free(curr_cmd.stdout_file);
            curr_cmd.stdout_file = strdup(curr_arg);
            expect = EXPECT_NOTHING;
        }
        else if (expect == EXPECT_STDERR_FILE)
        {
            if (curr_cmd.stderr_file != NULL) free(curr_cmd.stderr_file);
            curr_cmd.stderr_file = strdup(curr_arg);
            expect = EXPECT_NOTHING;
        }
        else curr_cmd.argv[argc++] = strdup(curr_arg);
        len = 0;
        curr_arg[0] = '\0';
    }
    curr_cmd.argv[argc] = NULL;

    return curr_cmd;

}
