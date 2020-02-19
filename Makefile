main: main.c mthread.c  switch.s mthread.h
	gcc -m32 -g main.c mthread.c switch.s -o main
