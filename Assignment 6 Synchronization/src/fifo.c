#include "fifo.h"

void fifo_init(struct fifo *f){
  cv *readMap = NULL;
  cv *writeMap = NULL;

  readMap = (cv *)mmap(NULL, sizeof(cv), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
  writeMap = (cv *)mmap(NULL, sizeof(cv), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
  if(readMap < 0){
	  fprintf(stderr, "Error: failed to mmap() anonymous page for read %s\n", strerror(errno));
	  exit(255);
  }
  if(writeMap < 0){
    fprintf(stderr, "Error: failed to mmap() anonymous page for write %s\n", strerror(errno));
    exit(255);
  }

  f->r = *readMap;
  f->readNext = 0;
  cv_init(&f->r);

  f->w = *writeMap;
  f->writeNext = 0;
  cv_init(&f->w);

  f->state = 0;
  f->lock.primlock = 0;
}

void fifo_wr(struct fifo *f, unsigned long d){
  spin_lock(&f->lock);

  while(f->state >= MYFIFO_BUFSIZ){
	  cv_wait(&f->w, &f->lock);
  }

  f->buf[f->writeNext++] = d;
  f->writeNext %= MYFIFO_BUFSIZ;
  f->state++;

  cv_signal(&f->r);
  spin_unlock(&f->lock);
}

unsigned long fifo_rd(struct fifo *f){
  unsigned long fifoRead;
  int i = 0;

  spin_lock(&f->lock);

  while(f->state <= 0){
    printf("Reader stream %d completed\n", i);
	  cv_wait(&f->r, &f->lock);
    i++;
  }

  fifoRead = f->buf[f->readNext++];
  f->readNext %= MYFIFO_BUFSIZ;
  f->state--;

  cv_signal(&f->w);
  spin_unlock(&f->lock);
  return fifoRead;
}
