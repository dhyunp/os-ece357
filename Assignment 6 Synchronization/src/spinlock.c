#include "spinlock.h"

void spin_lock(struct spinlock *l){
  while(tas(&(l->primlock)) != 0){
    ;
  }
}

void spin_unlock(struct spinlock *l){
  l->primlock = 0;
}
