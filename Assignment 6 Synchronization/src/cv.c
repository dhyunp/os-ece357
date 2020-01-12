#include "cv.h"

void sigHandle(int signum){
  ;
}

void cv_init(struct cv *cv){
  spinlock *lock;

  int *map = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0);

  if(map < 0){
    fprintf(stderr, "Error: failed to mmap() anonymous page - %s\n", strerror(errno));
    exit(255);
  }

  lock = (spinlock *)(map + sizeof(spinlock));
  cv->lock = *lock;

  for(int i = 0; i < 64; i++){
    cv->pid[i] = 0;
  }

  cv->i = 0;
  signal(SIGUSR1, sigHandle);
  sigfillset(&cv->sigMask);
  sigdelset(&cv->sigMask, SIGUSR1);
}

void cv_wait(struct cv *cv, struct spinlock *mutex){
  if(cv->i >= 64){
  	fprintf(stderr, "Error: too many processes\n");
  	exit(255);
  }

  spin_lock(&cv->lock);
  cv->pid[cv->i] = getpid();
  cv->i++;
  spin_unlock(&cv->lock);
  spin_unlock(mutex);

  sigprocmask(SIG_BLOCK, &cv->sigMask, NULL);
  sigsuspend(&cv->sigMask);

  if(cv->i > 0){
	  spin_lock(&cv->lock);
	  cv->pid[cv->i - 1] = 0;
	  cv->i--;
	  spin_unlock(&cv->lock);
	  spin_lock(mutex);
    return;
  }

  sigprocmask(SIG_UNBLOCK, &cv->sigMask, NULL);
  spin_lock(mutex);
}

int cv_broadcast(struct cv *cv){
  int wakeNum = 0;

  spin_lock(&cv->lock);

  if(cv->i == 0){
    spin_unlock(&cv->lock);
    return 0;
  }

  for(int j = 0; j < 64; j++){
    if(cv->pid[j] > 0){
      kill(cv->pid[j], SIGUSR1);
      wakeNum++;
    }
  }
  spin_unlock(&cv->lock);
  return wakeNum;
}

int cv_signal(struct cv *cv){
  int wakeNum = 0;

  spin_lock(&cv->lock);

  if(cv->i == 0){
    spin_unlock(&cv->lock);
    return 0;
  }

  kill(cv->pid[cv->i - 1], SIGUSR1);
  wakeNum++;
  spin_unlock(&cv->lock);
  return wakeNum;
}
