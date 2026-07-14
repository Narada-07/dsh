#include <dsh/command.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <dsh/redirections.h>
#include <stdio.h>
bool open_redirections(Command* cmd, int* fd_out, int* fd_err)
{
  *fd_out = *fd_err = -1;
  if (cmd->stdout_mode != INVALID_MODE)
  {
    if (cmd->stdout_file == NULL)
    {
        perror("open_redirections");
        return false;
    }
    if (cmd->stdout_mode == REDIRECT_TRUNCATE) *fd_out = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else *fd_out = open(cmd->stdout_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (*fd_out == -1)
    {
        perror("open_redirections");
        return false;
    }
  }
  if (cmd->stderr_mode != INVALID_MODE)
  {
    if (cmd->stderr_file == NULL)
    {
        perror("open_redirections");
        return false;
    }
    if (cmd->stderr_mode == REDIRECT_TRUNCATE) *fd_err = open(cmd->stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else *fd_err = open(cmd->stderr_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (*fd_err == -1)
    {
        if (*fd_out != -1)
            close(*fd_out);

        perror("open_redirections");
        return false;
    }
  }

  return true;

}

bool apply_redirections(int fd_out, int fd_err)
{
  if (fd_out != -1)
  if (dup2(fd_out, STDOUT_FILENO) == -1)
    {
      perror("apply_redirections");
      return false;
    }
  if (fd_err != -1)
    if (dup2(fd_err, STDERR_FILENO) == -1)
    {  
      perror("apply_redirections");
      return false;
    }
  if (fd_out != -1) close(fd_out);
  if (fd_err != -1) close(fd_err);
  return true;
}

bool setup_redirections(Command* cmd, int* fd_out, int* fd_err)
{
    if (open_redirections(cmd, fd_out, fd_err))
    {
        if (!apply_redirections(*fd_out, *fd_err))
        {
            if (*fd_out != -1) close(*fd_out);
            if (*fd_err != -1) close(*fd_err);
            return false;
        }
        return true;
    }
    return false;
}

bool save_stdio(int *saved_out, int *saved_err)
{
  *saved_out = dup(STDOUT_FILENO);
  *saved_err = dup(STDERR_FILENO);
  if (*saved_out == -1 || *saved_err == -1)
{
    if (*saved_out != -1)
        close(*saved_out);

    if (*saved_err != -1)
        close(*saved_err);

    perror("save_stdio");
    return false;
}

  return true;
}

bool restore_stdio(int saved_out, int saved_err)
{
  if (dup2(saved_out, STDOUT_FILENO) == -1 || dup2(saved_err, STDERR_FILENO) == -1)
  {
    perror("restore_stdio");
    return false;
  }
  close(saved_out);
  close(saved_err);
  return true;
}