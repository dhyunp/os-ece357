#include "spinlock.h"

typedef struct cv{
  int i;
  spinlock lock;
  pid_t pid[64];
  sigset_t sigMask;
}cv;

void cv_init(struct cv *cv);
void cv_wait(struct cv *cv, struct spinlock *mutex);
int cv_broadcast(struct cv *cv);
int cv_signal(struct cv *cv);
