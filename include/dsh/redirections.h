#ifndef REDIRECTIONS_H
#define REDIRECTIONS_H
#include <stdbool.h>
#include <dsh/command.h>

bool open_redirections(Command* cmd, int* fd_out, int* fd_err);
bool apply_redirections(int fd_out, int fd_err);
bool setup_redirections(Command* cmd, int* fd_out, int* fd_err);

bool save_stdio(int *saved_out, int *saved_err);
bool restore_stdio(int saved_out, int saved_err);

#endif