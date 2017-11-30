#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

int main(int argc, char **argv){
  int pipe1[2], pipe2[2], status; //0 read, 1 write
  pid_t cpid1, cpid2, cpid3, wpid1, wpid2, wpid3;
  struct rusage ru;

  if(pipe(pipe1) == -1){  //wordgen->wordsearch
    fprintf(stderr, "Failed to create pipe1 - %s\n", strerror(errno));
    return -1;
  }
  if(pipe(pipe2) == -1){  //wordsearch->pager
    fprintf(stderr, "Failed to create pipe2 - %s\n", strerror(errno));
    return -1;
  }
  switch(cpid1 = fork()){ //exec wordgen
    case -1:
      fprintf(stderr, "Error: Failed to fork() process 1 'wordgen' - %s\n", strerror(errno));
      exit(1);
      break;
    case 0: //child process, dup stdout(1) of wordgen to write/input of pipe1(pipe1[1])
      if(dup2(pipe1[1], 1) == -1){
        fprintf(stderr, "Error: Failed to dup2() fd 1 to input of pipe1 - %s\n", strerror(errno));
        return -1;
      }

      if(close(pipe1[0]) == -1){
        fprintf(stderr, "Error: Failed to close read end of pipe1 - %s\n", strerror(errno));
      }
      if(close(pipe1[1]) == -1){
        fprintf(stderr, "Error: Failed to close write end of pipe1 - %s\n", strerror(errno));
      }
      if(close(pipe2[0]) == -1){
        fprintf(stderr, "Error: Failed to close read end of pipe2 - %s\n", strerror(errno));
      }
      if(close(pipe2[1]) == -1){
        fprintf(stderr, "Error: Failed to close write end of pipe2 - %s\n", strerror(errno));
      }

      if(execlp("./wordgen", "wordgen", argv[1], (char *)NULL) == -1){
        fprintf(stderr, "Error: Failed to execute ./wordgen - %s\n", strerror(errno));
      }
      break;
    default:  //parent process
      switch(cpid2 = fork()){ //exec wordsearch
        case -1:
          fprintf(stderr, "Error: Failed to fork() process 2 'wordsearch' - %s\n", strerror(errno));
          exit(1);
          break;
        case 0: //child process, dup read/output of pipe1(pipe1[0]) to stdin(0) of wordsearch, stdout(1) of wordsearch to write/intput of pipe2(pipe2[1])
          if(dup2(pipe1[0], 0) == -1){
            fprintf(stderr, "Error: Failed to dup2() fd 0 to output of pipe1 - %s\n", strerror(errno));
            return -1;
          }
          if(dup2(pipe2[1], 1) == -1){
            fprintf(stderr, "Error: Failed to dup2() fd 1 to input of pipe2 - %s\n", strerror(errno));
            return -1;
          }

          if(close(pipe1[0]) == -1){
            fprintf(stderr, "Error: Failed to close read end of pipe1 - %s\n", strerror(errno));
          }
          if(close(pipe1[1]) == -1){
            fprintf(stderr, "Error: Failed to close write end of pipe1 - %s\n", strerror(errno));
          }
          if(close(pipe2[0]) == -1){
            fprintf(stderr, "Error: Failed to close read end of pipe2 - %s\n", strerror(errno));
          }
          if(close(pipe2[1]) == -1){
            fprintf(stderr, "Error: Failed to close write end of pipe2 - %s\n", strerror(errno));
          }

          if(execlp("./wordsearch", "wordsearch", "dictionary.txt", (char *)NULL) == -1){
            fprintf(stderr, "Error: Failed to execute ./wordsearch - %s\n", strerror(errno));
          }
          break;
        default:  //parent process
          switch(cpid3 = fork()){ //exec pager
            case -1:
              fprintf(stderr, "Error: Failed to fork() process 3 'pager' - %s\n", strerror(errno));
              exit(1);
              break;
            case 0: //child process, dup read/output of pipe2(pipe[0]) to stdin(0) of pager
              if(dup2(pipe2[0], 0) == -1){
                fprintf(stderr, "Error: Failed to dup2() fd 0 to output of pipe2 - %s\n", strerror(errno));
                return -1;
              }

              if(close(pipe1[0]) == -1){
                fprintf(stderr, "Error: Failed to close read end of pipe1 - %s\n", strerror(errno));
              }
              if(close(pipe1[1]) == -1){
                fprintf(stderr, "Error: Failed to close write end of pipe1 - %s\n", strerror(errno));
              }
              if(close(pipe2[0]) == -1){
                fprintf(stderr, "Error: Failed to close read end of pipe2 - %s\n", strerror(errno));
              }
              if(close(pipe2[1]) == -1){
                fprintf(stderr, "Error: Failed to close write end of pipe2 - %s\n", strerror(errno));
              }

              if(execlp("./pager", "./pager", (char *)NULL) == -1){
                fprintf(stderr, "Error: Failed to execute ./pager - %s\n", strerror(errno));
              }
              break;
          }
          break;
      }
      break;
  }

  if(close(pipe1[0]) == -1){
    fprintf(stderr, "Error: Failed to close read end of pipe1 - %s\n", strerror(errno));
  }
  if(close(pipe1[1]) == -1){
    fprintf(stderr, "Error: Failed to close write end of pipe1 - %s\n", strerror(errno));
  }
  if(close(pipe2[0]) == -1){
    fprintf(stderr, "Error: Failed to close read end of pipe2 - %s\n", strerror(errno));
  }
  if(close(pipe2[1]) == -1){
    fprintf(stderr, "Error: Failed to close write end of pipe2 - %s\n", strerror(errno));
  }

  if((wpid1 = wait3(&status, 0, &ru)) < 0){
    fprintf(stderr, "Error: Failed to wait() process on pid=[%d] - %s\n", cpid1, strerror(errno));
  }else{
    fprintf(stderr, "Child 1 %d exited with %d\n", wpid1, status);
  }
  if((wpid2 = wait3(&status, 0, &ru)) < 0){
    fprintf(stderr, "Error: Failed to wait() process on pid=[%d] - %s\n", cpid2, strerror(errno));
  }else{
    fprintf(stderr, "Child 2 %d exited with %d\n", wpid2, status);
  }
  if((wpid3 = wait3(&status, 0, &ru)) < 0){
    fprintf(stderr, "Error: Failed to wait() process on pid=[%d] - %s\n", cpid3, strerror(errno));
  }else{
    fprintf(stderr, "Child 3 %d exited with %d\n", wpid3, status);
  }

  return 0;
}
