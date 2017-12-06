#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>

void sigHandle(int sig){
  fprintf(stderr, "Signal %d, [%s] detected\n", sig, strsignal(sig));
  exit(sig);
}

int makeFile(size_t size){
  int fd;
  char buf[1];

  if((fd = open("test.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0){
    fprintf(stderr, "Error: opening/creating test.txt file - %s\n", strerror(errno));
    exit(255);
  }
  srand(time(NULL));

  for(int i = 0; i < size; i++){
    buf[0] = rand() % 26 + 65;
    if(write(fd, buf, 1) < 0){
      fprintf(stderr, "Error: writing to test.txt file - %s\n", strerror(errno));
      exit(255);
    }
  }

  return fd;
}

void t1(){
  char *mapped;
  size_t len = 4096;
  int fd = makeFile(len);

  printf("Executing Test #1 (write to r/o mmap):\n");

  if((mapped = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "Error: failed to mmap() file to memory - %s\n", strerror(errno));
    close(fd); //unnecesarry since exiting program cleans up file descriptor table, included here for demonstration purposes
    exit(255);
  }

  printf("map[3] == \'%c\'\n", mapped[3]);
  printf("writing a \'%c\'\n", mapped[4]);

  mapped[3] = mapped [4];

  if(munmap(mapped,len) < 0){
    fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
    exit(255);
  }

  exit(0);
}

int t23(int mapFlags){
  char* mapped;
  char buf[1];
  size_t len = 4096;
  int fd = makeFile(len), offset = 25, reflect;

  if(mapFlags == MAP_SHARED){
    printf("Executing Test #2 (write to MAP_SHARED mmap):\n");
  }else if(mapFlags == MAP_PRIVATE){
    printf("Executing Test #2 (write to MAP_PRIVATE mmap):\n");
  }else{
    fprintf(stderr, "Error: Invalid map flags\n");
  }

  if((mapped = mmap(NULL, len, PROT_READ|PROT_WRITE, mapFlags, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "Error: failed to mmap() file to memory - %s\n", strerror(errno));
    exit(255);
  }

  printf("map[%d] == \'%c\'\n", offset, mapped[offset]);
  printf("writing a \'T\'\n");

  mapped[offset] = 'T';

  printf("updated map[%d] == \'%c\'\n", offset, mapped[offset]);

  if(lseek(fd, offset, SEEK_SET) < 0){
    fprintf(stderr, "Error: failed to lseek() on fd[%d] - %s\n", fd, strerror(errno));
  }
  if(read(fd, buf, 1) < 0){
    fprintf(stderr, "Error: failed to read from fd[%d] - %s\n", fd, strerror(errno));
  }

  printf("read from file returns \'%c\'\n", buf[0]);

  if(buf[0] == 'T'){
    reflect = 0;
  }else{
    reflect = 1;
  }

  printf("Update to the mapped memory with %s %s\n", mapFlags==MAP_SHARED?"MAP_SHARED":"MAP_PRIVATE", reflect? "is not visible.":"is visible.");

  if(munmap(mapped,len) < 0){
    fprintf(stderr, "Error: failed to munmap() from memory - %s\n", strerror(errno));
    exit(255);
  }

  reflect? exit(0):exit(1);
}

int t4(){
  char* mapped;
  size_t len = 5000;
  int fd = makeFile(len), offset;
  struct stat sb;
  off_t size1, size2;

  printf("Executing Test #4 (writing beyond the edge):\n");

  if(fstat(fd, &sb) < 0){
    fprintf(stderr, "Error: failed to fstat() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }

  size1 = sb.st_size;
  printf("Previous File Size = %ld bytes\n", size1);

  if((mapped = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "Error: failed to mmap() to memory - %s\n", strerror(errno));
    exit(255);
  }

  offset = len;
  mapped[offset] = 'T'; //if file is 5000 bytes long, the last byte will be at 4999

  if(fstat(fd, &sb) < 0){
    fprintf(stderr, "Error: failed to fstat() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }

  if((size2 = sb.st_size) == size1){
    printf("New File Size = %ld bytes, equivalent\n", size2);
    if(munmap(mapped, len) < 0){
      fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
      exit(255);
    }
    exit(1);
  }else{
    printf("New File Size = %ld bytes, changed\n", size2);
    if(munmap(mapped, len) < 0){
      fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
      exit(255);
    }
    exit(0);
  }

}

int t5(){
  char* mapped;
  size_t len = 5000;
  int fd = makeFile(len), offset;
  char buf[1];
  struct stat sb;

  printf("Executing Test #5 (writing into a hole):\n");

  if((mapped = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "Error: failed to mmap() file to memory - %s\n", strerror(errno));
    exit(255);
  }

  offset = len;
  mapped[offset] = 'T';
  buf[0] = 'A';

  if(lseek(fd, len+16, SEEK_SET) < 0){
    fprintf(stderr, "Error: failed to lseek() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }
  if(write(fd, buf, 1) < 0){
    fprintf(stderr, "Error: failed to write() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }
  if(lseek(fd, len, SEEK_SET) < 0){
    fprintf(stderr, "Error: failed to lseek() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }
  if(read(fd, buf, 1) < 0){
    fprintf(stderr, "Error: failed to read() file descriptor [%d] - %s\n", fd, strerror(errno));
    exit(255);
  }

  printf("Read from \'one byte beyond last byte\': %c\n", buf[0]);

  if(buf[0] == 'T'){
    if(munmap(mapped, len) < 0){
      fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
      exit(255);
    }
    printf("Writing into a hole is visible\n");
    exit(0);
  }else{
    if(munmap(mapped, len) < 0){
      fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
      exit(255);
    }
    printf("Writing into a hole is not visible\n");
    exit(1);
  }
}

int t6(){
  size_t len = 2500;
  int fd = makeFile(len), offset;
  char* mapped;

  printf("Executing Test #6 (reading beyond the edges):\n");

  if((mapped = mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    fprintf(stderr, "Error: failed to mmap() file to memory - %s\n", strerror(errno));
    exit(255);
  }

  fprintf(stderr, "map[3000]: \'%d\'\n", mapped[3000]);
  fprintf(stderr, "map[5000]: \'%d\'\n", mapped[5000]);

  if(munmap(mapped, len) < 0){
    fprintf(stderr, "Error: failed to munmap() file from memory - %s\n", strerror(errno));
    exit(255);
  }

  exit(0);
}

int main(int argc, char **argv){
  int i = 1;

  while(i < 32){
    signal(i, sigHandle);
    i++;
  }

  if(argc < 2){
    fprintf(stderr, "No Memory Test Specified: Type a number from [1 - 6]\n");
    exit(255);
  }

  switch(atoi(argv[1])){
    case 1:
      t1();
      break;
    case 2:
      t23(MAP_SHARED);
      break;
    case 3:
      t23(MAP_PRIVATE);
      break;
    case 4:
      t4();
      break;
    case 5:
      t5();
      break;
    case 6:
      t6();
      break;
  }
}
