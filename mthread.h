#ifndef __THREAD_H__
#define __THREAD_H__

#define N_TASKS 10				//开辟的最大线程数
#define STACK_SIZE 1024    		//栈大小
#define RUNNING 1
#define SLEEP 2
#define EXIT -1

typedef struct task_struct {
  int id;                    //线程id
  void (*th_fn)();           //线程函数
  int esp; 					 // 保存栈顶指针
  int wakeuptime;            // 线程唤醒时间
  int status; 				 // 线程状态
  int timeslice; 			 // 时间片
  int priority; 			 // 线程优先级
  int stack[STACK_SIZE];
}mtask,*ptask;

int mthread_create(int *tid, void (*th_fn)());
int mthread_join(int tid);
void msleep(int s);

#endif //__THREAD_H__
