#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include <dsh/path.h>
#include <stddef.h>
#include <unistd.h>



int findExecutable(char* path, char* arg, char* candidate, int buffer_size)
{
  char* path_copy = strdup(path);
  char* dir = strtok(path_copy, ":");
  int find_path = 0;

          //For exec search
  while (dir != NULL)
  {
    snprintf(candidate, buffer_size, "%s/%s", dir, arg);
    if (access(candidate, F_OK) == 0)
    {
      if (access(candidate, X_OK) == 0)
      {
        find_path = 1;
        break;
      }
    }
    dir = strtok(NULL, ":");
  }

  free(path_copy);
  return find_path;
}