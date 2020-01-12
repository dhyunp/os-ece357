#include "fifo.h"

int main(int argc, char **argv){
  int writers, items;
  fifo *fifo1;
  pid_t pid1, pid2;

  if(argc < 3){
    printf("Error: please input 3 arguments\n");
    exit(255);
  }

  writers = atoi(argv[1]+2);
  items = atoi(argv[2]+2);
  printf("Beginning test with %d writers, %d items each\n", writers, items);

  fifo1 = (fifo *)mmap(NULL, sizeof(fifo), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
  if(fifo1 < 0){
    fprintf(stderr, "Error: failed to mmap() anonymous page for fifo - %s\n", strerror(errno));
    exit(255);
  }

  fifo_init(fifo1);

  for(int i = 0; i < writers; i++){
    switch(pid1 = fork()){
      case -1:
        fprintf(stderr, "Error: failed to fork() #%dth time - %s\n", i, strerror(errno));
        break;
      case 0:;
        unsigned long writeBuf[items];

        for(int j = 0; j < items; j++){
          writeBuf[j] = getpid()*10000 + j;
          fifo_wr(fifo1, writeBuf[j]);
        }

        printf ("Write %d completed\n", i);
        exit(0);
        break;
    }
  }

  switch(pid2 = fork()){
    case -1:
      fprintf(stderr, "Error: failed to fork() - %s\n", strerror(errno));
      break;
    case 0:;
      unsigned long readBuf[writers * items];

      for(int i = 0; i < (writers * items); i++){
        readBuf[i] = fifo_rd(fifo1);
      }

      printf("All streams done\n");
      break;
  }

  printf("Waiting for writer children to die\n");

  for(int i = 0; i < writers + 1; i++){
    wait(0);
  }

  return 0;
}
