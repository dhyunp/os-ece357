#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, int **argv){
  ssize_t read;
  size_t len = 0;
  char *command, *line = NULL;
  int count = 0;
  int c;
  FILE *dev;

  while((read = getline(&line, &len, stdin)) != -1){
    printf("%s", line);
    count++;
    if(count == 23){
      printf("---Press RETURN for more---");
      if((dev = fopen("/dev/tty", "r+")) == NULL){
        fprintf(stderr, "Error: Failed to open file [/dev/tty] - %s\n", strerror(errno));
        return -1;
      }

      c = getc(dev);

      if(c == 113 || c == 81){
        exit(0);
      }
      count = 0;
      fclose(dev);
    }
  }
  return 0;
}
