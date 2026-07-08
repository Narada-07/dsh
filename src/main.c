#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dsh/lexer.h>

int findExecutable(char* path, char* arg, char* candidate, int buffer_size)
{
  char* path_copy = strdup(path);
  char* dir = strtok(path_copy, ":");
  int findPath = 0;

          //For exec search
  while (dir != NULL)
  {
    snprintf(candidate, buffer_size, "%s/%s", dir, arg);
    if (access(candidate, F_OK) == 0)
    {
      if (access(candidate, X_OK) == 0)
      {
        findPath = 1;
        break;
      }
    }
    dir = strtok(NULL, ":");
  }

  free(path_copy);
  return findPath;
}


int main(int program_argc, char *program_argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char *path = getenv("PATH");
  char *home = getenv("HOME");
  // TODO: Uncomment the code below to pass the first stage
  while(1)
  {
    printf("$ ");
    char input[1024];

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';


    if (input[0] == '\0') continue;
    //Tokenize
    char *argv[100];
    tokenize(input, argv);

    

    //Exit
    if (strcmp(argv[0], "exit") == 0)
      break;
    
    //Echo
    else if (strcmp(argv[0], "echo") == 0)
    {
      for (int i = 1; argv[i] != NULL; i++)
      {
        printf("%s", argv[i]);

        if (argv[i + 1] != NULL)
        {
          printf(" ");
        }
      }
      printf("\n");
    }

    // Type
    else if (strcmp(argv[0], "type") == 0)
    {
      for(int i = 1; argv[i] != NULL; i++)
      { 
        if (!strcmp(argv[i], "exit") || !strcmp(argv[i], "echo") || !strcmp(argv[i], "type") || !strcmp(argv[i], "pwd")) //For builtin
          printf("%s is a shell builtin\n", argv[i]);
        else 
        {
          char candidate[1024];
          int findPath = findExecutable(path, argv[i], candidate, 1024);
          if (findPath) printf("%s is %s\n", argv[i], candidate);
          //Not found
          if (!findPath) printf("%s: not found\n", argv[i]); 
        }
      }
    }
    else if (strcmp(argv[0], "pwd") == 0)
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
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
      char* address;
      if (strcmp(argv[1], "~") == 0) address = home;
      else address = argv[1];

      if (chdir(address) == 0);
      else
      {
        fprintf(stderr, "cd: %s: No such file or directory\n", argv[1]);
      }

    }
    else
    {
      char candidate[1024];
      int findPath = findExecutable(path, argv[0], candidate, 1024);
      if (findPath)
      {
        pid_t pid = fork();
        if (pid == 0)
        {
          execv(candidate, argv);

          perror("execv");
          return 1;
        }
        else waitpid(pid, NULL, 0);
      }
      //Not found anything
      if (!findPath) printf("%s: command not found\n", argv[0]);
    }

    for (int i = 0; argv[i] != NULL; ++i) free(argv[i]);
  }
  return 0;
}
