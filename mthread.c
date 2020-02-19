#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "mthread.h"

static mtask init_task = {0, NULL, RUNNING, 0, 0, 15, 15, {0}};
ptask cur = &init_task;
ptask task[N_TASKS] = {&init_task,};  
 
void schedule();
static void start(ptask tsk) 
{
  tsk->th_fn();
  tsk->status = EXIT;
  printf("***Thread%d exited***\n", tsk->id);
  schedule();
}

int mthread_create(int *tid, void (*th_fn)()) {
  int id = -1;
  ptask tsk = (ptask)malloc(sizeof(mtask));
  while(++id<N_TASKS && task[id]);
  if (id == N_TASKS){
	  free(tsk);
    return -1;
  }
  task[id] = tsk;
  if(tid) 
	  *tid = id;
  //初始化线程结构体
  tsk->id = id;
  tsk->th_fn = th_fn;
  int *stack = tsk->stack;
  tsk->esp = (int)(stack+STACK_SIZE-11);
  tsk->wakeuptime = 0;
  tsk->status = RUNNING;
  tsk->timeslice = 15;
  tsk->priority = 15;
  // 初始栈帧
  stack[STACK_SIZE-11] = 7; // eflags
  stack[STACK_SIZE-10] = 6; // eax
  stack[STACK_SIZE-9] = 5; // edx
  stack[STACK_SIZE-8] = 4; // ecx
  stack[STACK_SIZE-7] = 3; // ebx
  stack[STACK_SIZE-6] = 2; // esi
  stack[STACK_SIZE-5] = 1; // edi
  stack[STACK_SIZE-4] = 0; // old ebp
  stack[STACK_SIZE-3] = (int)start;  
  stack[STACK_SIZE-2] = 10;  
  stack[STACK_SIZE-1] = (int)tsk; // start 的参数
  return 0;
}

int mthread_join(int tid) 
{
  while(task[tid]->status != EXIT) {
		schedule();
  }
  free(task[tid]);
  task[tid] = NULL;
}

static unsigned int getmstime() 
{
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0) {
     perror("gettimeofday");
     exit(-1);
  }
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
static ptask pick() //linus
{
  int i, next, c;
  for (i = 0; i < N_TASKS; ++i) {
    if (task[i] && task[i]->status != EXIT && getmstime() > task[i]->wakeuptime){
      task[i]->status = RUNNING;
    }
  }
  while(1) {
    c = -1;
    next = 0;
    for (i = 0; i < N_TASKS; ++i) {
      if (!task[i]) continue;
      if (task[i]->status == RUNNING && task[i]->timeslice > c) {
        c = task[i]->timeslice;
        next = i;
      }
    }
    if (c) break;
    if (c == 0) {                       
      for (i = 0; i < N_TASKS; ++i) {
        if(task[i]) {
          task[i]->timeslice = task[i]->priority + (task[i]->timeslice >> 1);
        }
      }
    }
  }
  return task[next];
}

void switch_to(ptask next);
void schedule() 
{
    ptask next= pick();
    if (next) {
      switch_to(next);
    }
}

void msleep(int s)        
{
	cur->wakeuptime = getmstime() + 1000*s;
	cur->status = SLEEP;
	schedule();
}


static void mtimer() 			//时间片监控
{
	if (--cur->timeslice > 0) 
	return;
	cur->timeslice = 0;
	schedule();
}

__attribute__((constructor))
static void init()                    //时间片信号控制
{
  struct itimerval value;
  value.it_value.tv_sec = 0;
  value.it_value.tv_usec = 1000;
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = 1000*10; // 10 ms
  if (setitimer(ITIMER_REAL, &value, NULL) < 0) {
    perror("setitimer");
  }
  signal(SIGALRM, mtimer);
}

void closealarm() 
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
      perror("sigprocmask BLOCK");
    }
}
void openalarm() 
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
      perror("sigprocmask BLOCK");
    }
}


