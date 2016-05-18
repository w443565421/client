#include<stdio.h>  
#include<fcntl.h>  
#include<unistd.h>  
#include "lock.h"

//lock.c
int lock_set(int fd,int type){
  struct flock lock;

  lock.l_type=type;
  lock.l_start=0;
  lock.l_whence=SEEK_SET;
  lock.l_len = 0;
  lock.l_pid=-1;

  fcntl(fd,F_GETLK,&lock);
  if(lock.l_type!=F_UNLCK){
    if(lock.l_type == F_RDLCK)
      printf("Read lock already set by %d!\n",lock.l_pid);
    else if(lock.l_type == F_WRLCK)
      printf("Write lock already set by %d!\n",lock.l_pid);
  }
  lock.l_type = type;
  //此处的F_SETLKW为F_SETLK的阻塞版本，当无法获取锁时进入睡眠等待状态
  if(fcntl(fd,F_SETLKW,&lock)<0){
    printf("Lock failed:type=%d!\n",lock.l_type);
    exit(1);
  }

  switch(lock.l_type){
  case F_RDLCK:
    printf("read lock set by %d\n",getpid());
    break;
  case F_WRLCK:
    printf("write lock set by %d\n",getpid());
    break;
  case F_UNLCK:
    printf("UN lock set by %d\n",getpid());
    break;
  default:
    break;
  }
}
