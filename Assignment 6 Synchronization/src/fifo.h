#include "cv.h"

#define MYFIFO_BUFSIZ 1024

typedef struct fifo{
  int state, readNext, writeNext;
  unsigned long buf[MYFIFO_BUFSIZ];
  spinlock lock;
  cv w, r;
}fifo;

void fifo_init(struct fifo *f);
void fifo_wr(struct fifo *f, unsigned long d);
unsigned long fifo_rd(struct fifo *f);
