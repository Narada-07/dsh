#ifndef BUILTIN_H
#define BUILTIN_H
#include <dsh/command.h>

typedef int (*BuiltinFn)(Command*, int*);

typedef struct Builtin
{
    const char *name;
    BuiltinFn fn;
}Builtin;

int isBuiltin(char *arg);

int runBuiltin(Command* cmd, int* running);

#endif