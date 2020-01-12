#include "spinlock.h"

int main(int argc, char **argv){
  long long unsigned int nchild, niter;
  pid_t pid;

  if(argc != 3){
    fprintf(stderr, "Error: Please input 3 arguments\n");
    exit(255);
  }

  nchild = atoll(argv[1]);
  niter = atoll(argv[2]);

  int *map = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0);
  if(map < 0){
    fprintf(stderr, "Error: failed to mmap() anonymous page - %s\n", strerror(errno));
    exit(255);
  }

  map[0] = 0;
  spinlock *lock;
  lock = (spinlock *)(map + sizeof(spinlock));
  lock -> primlock = map[1];

  for(int i = 0; i < nchild; i++){
    switch(pid = fork()){
      case -1:
        fprintf(stderr, "Error: failed to fork() #%dth time - %s\n", i, strerror(errno));
        break;
      case 0:
        spin_lock(lock);
        for(int j = 0; j < niter; j++){
          map[0]++;
        }
        spin_unlock(lock);
        exit(0);
        break;
    }
  }

  for(int i = 0; i < nchild; i++){
    wait(0);
  }

  printf("value of map[0] = %d\n", map[0]);
  return 0;
}
