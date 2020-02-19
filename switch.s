.section .text
.global switch_to
switch_to:
  call closealarm   /* 模拟关中断 */
  push %ebp
  mov %esp, %ebp 
  /* 保存现场 */
	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
  pushfl
 
  /* 准备切换栈 */
  mov cur, %eax      /* 取当前线程指针基址放到 eax */
  mov %esp, 8(%eax)  /* 保存当前esp到线程结构体 */ 
  mov 8(%ebp), %eax  /* 取下一个线程结构体基址*/
  mov %eax, cur      /* 更新当前线程指针基址*/
  mov 8(%eax), %esp  /* 切换到下一个线程的栈 */

  /* 恢复现场 */
  popfl
	popl %eax
	popl %edx
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
  call openalarm    /* 模拟开中断  */
  ret
