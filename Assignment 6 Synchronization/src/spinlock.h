#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct spinlock{
  volatile char primlock;
}spinlock;

int tas(volatile char *lock);

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);
