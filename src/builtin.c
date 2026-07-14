#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <dsh/command.h>
#include <dsh/builtin.h>
#include <dsh/path.h>

int builtin_exit(Command* cmd, int* running)
{
    *running = 0;
    return 0;
}

int builtin_echo(Command* cmd, int* running)
{
    for (int i = 1; cmd->argv[i] != NULL; i++)
    {
    printf("%s", cmd->argv[i]);

    if (cmd->argv[i + 1] != NULL)
    {
        printf(" ");
    }
    }
    printf("\n");

    return 0;
}

int builtin_type(Command* cmd, int *running)
{
    for(int i = 1; cmd->argv[i] != NULL; i++)
    { 
    //TODO: Replace with isBuiltin?
        if (isBuiltin(cmd->argv[i])) //For builtin
            printf("%s is a shell builtin\n", cmd->argv[i]);
        else 
        {
            char candidate[1024];
            int findPath = findExecutable(getenv("PATH"), cmd->argv[i], candidate, 1024);
            if (findPath) printf("%s is %s\n", cmd->argv[i], candidate);
            //Not found
            if (!findPath) printf("%s: not found\n", cmd->argv[i]); 
        }
    }
    return 0;
}

int builtin_pwd(Command* cmd, int* running)
{
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
    printf("%s\n", buffer);
    }
    else
    {
    perror("getcwd");
    }
    return 0;
}

int builtin_cd(Command* cmd, int* running)
{
    char* address;
    if (cmd->argv[1] == NULL) return 0;
    if (strcmp(cmd->argv[1], "~") == 0) address = getenv("HOME");
    else address = cmd->argv[1];

    if (chdir(address) == 0);
    else
    {
    fprintf(stderr, "cd: %s: No such file or directory\n", cmd->argv[1]);
    }

    return 0;
}

Builtin builtins[] = 
{
    {"exit", builtin_exit},
    {"echo", builtin_echo},
    {"type", builtin_type},
    {"pwd", builtin_pwd},
    {"cd", builtin_cd},
};


int runBuiltin(Command* cmd, int* running)
{
    int n = sizeof(builtins)/sizeof(builtins[0]);
    for (int i = 0; i < n; ++i) if (!strcmp(builtins[i].name, cmd->argv[0])) return builtins[i].fn(cmd, running);

    return 0;
}


int isBuiltin(char* arg)
{
    int n = sizeof(builtins)/sizeof(builtins[0]);
    for (int i = 0; i < n; ++i) if (!strcmp(builtins[i].name, arg)) return 1;

    return 0;
}