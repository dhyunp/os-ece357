#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <string.h>

int main(int argc, char **argv){
  int wordCount, length, totalNum;
  char nc[11]; //10 char + null terminator
  time_t t;
  srand((unsigned) time(&t));


  if(argc > 1){
    if(!strcmp(argv[1], "0")){
      wordCount = INT_MAX; //specifying 0 results in 'infinite' word generation
    }else{
      wordCount = atoi(argv[1]);
    }
  }else{
    wordCount = INT_MAX; //no argument results in 'infinite' word generation
  }

  totalNum = wordCount;

  for(; wordCount > 0; wordCount--){
    length = 3 + (rand() % 8); //min 3 characters in a word, 0-7 possible numbers, last index reserved for null terminator
    for(int i = 0; i < length; i++){
      nc[i] = (char)(65 + (rand() % 26));
    }
    nc[length] = '\0';
    printf("%s\n", nc);
  }

  fprintf(stderr, "Finished generating %d candidate words\n", totalNum);
  return 0;
}
