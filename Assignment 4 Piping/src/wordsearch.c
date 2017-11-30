#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

int matches;

void sigHandle(){
  fprintf(stderr, "Matched %d words\n", matches);
  exit(0);
}

void upperCase(char *word){
  for(int h = 0; h < strlen(word); h++){
    word[h] = toupper(word[h]);
  }
}

int main(int argc, char **argv){
  FILE *inputFile;
  size_t len1 = 0, len2 = 0;
  ssize_t read;
  char *buf[250000], *line = NULL;
  int i = 0;
  signal(SIGPIPE, sigHandle);

  if(argc < 2){
    fprintf(stderr, "Error: No intput file specified\n");
    return -1;
  }else{
    if((inputFile = fopen(argv[1], "r")) == NULL){
      fprintf(stderr, "Error: Failed to open file [%s] - %s\n", argv[1], strerror(errno));
      return -1;
    }
  }

  while((read = getline(&buf[i], &len1, inputFile)) != -1){
    upperCase(buf[i]);
    i++;
  }

  while((read = getline(&line, &len2, stdin)) != -1){
    upperCase(line);
    for(int j = 0; j < i; j++){
      if(!strcmp(line, buf[j])){
        printf("%s", line);
        matches++;
        break;
      }
    }
  }

  for(int k = 0; k < i; k++){
    free(buf[k]);
  }

  printf("Matched %d words\n", matches);
  fclose(inputFile);
  return 0;
}
