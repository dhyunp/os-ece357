.text
.global _start

_start:
  mov $1,%rax     #write system call
  mov $1,%rdi     #output to fd 1(stdout)
  mov $msg,%rdx   #msg to output
  mov $14,%rdx    #legnth of msg
  syscall
  mov $1,%rdi     #return code for exit syscall
  mov $60,%rax    #exit system call
  syscall
  .data

msg:
  .ascii "hello, world!\n"
