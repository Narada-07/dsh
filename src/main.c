#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dsh/lexer.h>
#include <dsh/command.h>
#include <dsh/path.h>
#include <stdbool.h>
#include <dsh/redirections.h>
#include <dsh/builtin.h>

void free_command(Command* cmd)
{
  for (int i = 0; cmd->argv[i] != NULL; ++i) free(cmd->argv[i]);
  if (cmd->stdout_file != NULL) free(cmd->stdout_file);
  if (cmd->stderr_file != NULL) free(cmd->stderr_file);
  return;
}

bool run_executable(Command* cmd, int* fd_out, int* fd_err)
{
  char candidate[1024];
  int findPath = findExecutable(getenv("PATH"), cmd->argv[0], candidate, 1024);
  if (findPath)
  {
    pid_t pid = fork();
    if (pid == 0)
    {
      if (!setup_redirections(cmd, fd_out, fd_err))
      {
        free_command(cmd);
        _exit(EXIT_FAILURE);
      }
      execv(candidate, cmd->argv);

      perror("execv");
      _exit(EXIT_FAILURE);
    }
    else if(pid == -1)
    {
      perror("run_executable");
      return false;
    }
    else waitpid(pid, NULL, 0);
  }
  if (!findPath) printf("%s: command not found\n", cmd->argv[0]);

  return true;
}

int main(int program_argc, char *program_argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);


  int running = 1;
  // TODO: Uncomment the code below to pass the first stage
  while(running)
  {
    printf("$ ");
    char input[1024];

    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    input[strcspn(input, "\n")] = '\0';


    if (input[0] == '\0') continue;
    //Tokenize

    Command cmd = tokenize(input);

    int fd_out;
    int fd_err;
    int saved_out;
    int saved_err;
    if (isBuiltin(cmd.argv[0]))
    {

      if (!save_stdio(&saved_out, &saved_err) || !setup_redirections(&cmd, &fd_out, &fd_err))
      {
        free_command(&cmd);
        continue;
      }

      runBuiltin(&cmd, &running);

      if(!restore_stdio(saved_out, saved_err)) break;
    }
    else run_executable(&cmd, &fd_out, &fd_err);

    free_command(&cmd);
  }
  return 0;
}