#ifndef COMMAND_H
#define COMMAND_H

typedef enum RedirectMode
{
    INVALID_MODE = -1,
    REDIRECT_TRUNCATE,
    REDIRECT_APPEND
}RedirectMode;

typedef struct 
{
    char *argv[100];

    char *stdout_file;
    char *stderr_file;

    RedirectMode stdout_mode;
    RedirectMode stderr_mode;
}Command;

#endif